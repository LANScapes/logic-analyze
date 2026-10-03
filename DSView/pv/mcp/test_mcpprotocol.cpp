/*
 * This file is part of the Logic Analyze project (Mac App Store edition).
 *
 * Copyright (C) 2026 Lanscapes
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Hardware-free test of mcpprotocol.cpp: framing, messages and the GUI lease.
//   cmake --build build --target mcp_protocol_test && build/mcp_protocol_test

#include "mcpprotocol.h"

#include <QJsonDocument>
#include <cstdio>

using namespace pv::mcp;

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while (0)

static QJsonObject agent(const char *json)
{
    return QJsonDocument::fromJson(QByteArray(json)).object();
}

static QByteArray raw_frame(const QByteArray &body)
{
    QByteArray f;
    quint32 n = body.size();
    f.append(char(n >> 24)); f.append(char(n >> 16)); f.append(char(n >> 8)); f.append(char(n));
    return f + body;
}

static void test_framing()
{
    QByteArray f = encode_frame(hello_message());
    CHECK(f.size() > 4);
    quint32 n = (quint8(f[0]) << 24) | (quint8(f[1]) << 16) | (quint8(f[2]) << 8) | quint8(f[3]);
    CHECK(n == quint32(f.size() - 4));

    // Two frames arriving in pieces.
    QByteArray buf = f + encode_frame(lease_message("released"));
    QByteArray in;
    QJsonObject m;
    bool err = false;
    int got = 0;
    for (char c : buf) {
        in.append(c);
        while (take_frame(in, m, err))
            got++;
        CHECK(!err);
    }
    CHECK(got == 2);
    CHECK(m.value("type").toString() == "lease" && m.value("op").toString() == "released");
    CHECK(in.isEmpty());

    // Zero length, oversize and non-object bodies are errors.
    QByteArray zero(4, '\0');
    CHECK(!take_frame(zero, m, err) && err);
    QByteArray big = raw_frame(QByteArray());
    big[0] = 0; big[1] = 0; big[2] = char(0x80); big[3] = 0;   // 32 KiB > the limit
    CHECK(!take_frame(big, m, err) && err);
    QByteArray arr = raw_frame("[1]");
    CHECK(!take_frame(arr, m, err) && err);
    QByteArray bad = raw_frame("{\"v\":");
    CHECK(!take_frame(bad, m, err) && err);

    // A body over the limit is not encoded.
    QJsonObject huge = lease_message("busy");
    huge["pad"] = QString(kMaxFrameBody, 'x');
    CHECK(encode_frame(huge).isEmpty());
}

static void test_messages()
{
    QJsonObject h = hello_message();
    CHECK(h.size() == 6);
    CHECK(h.value("v").toInt() == 1 && h.value("type").toString() == "gui_hello");
    CHECK(h.value("proto").toInt() == 1 && h.value("epoch").toInt() == kSecurityEpoch);
    CHECK(h.value("build").toInt() == kGuiBuild && h.value("min_build").toInt() == kGuiMinAgentBuild);
    // Integers go out as JSON integers, never 1.0.
    CHECK(encode_frame(h).contains("\"v\":1,") || encode_frame(h).contains("\"v\":1}"));

    QJsonObject l = lease_message("reclaim");
    CHECK(l.size() == 3 && l.value("type").toString() == "lease" && l.value("op").toString() == "reclaim");

    AgentMessage a = parse_agent_message(agent("{\"v\":1,\"type\":\"gui_ok\",\"build\":2,\"min_build\":1}"));
    CHECK(a.type == AgentMessage::Ok && a.build == 2 && a.min_build == 1);
    CHECK(parse_agent_message(agent("{\"v\":2,\"type\":\"gui_ok\",\"build\":1,\"min_build\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"gui_ok\",\"build\":1.5,\"min_build\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"gui_ok\",\"build\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"gui_error\",\"code\":\"version\"}")).type == AgentMessage::Error);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"lease_request\"}")).type == AgentMessage::LeaseRequest);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"lease_returned\"}")).type == AgentMessage::LeaseReturned);
    CHECK(parse_agent_message(agent("{\"type\":\"lease_request\"}")).type == AgentMessage::Invalid);
    // Messages that no longer exist are invalid.
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"evidence_result\",\"result\":\"granted\",\"expires_at\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"status\"}")).type == AgentMessage::Invalid);
}

static void test_lease()
{
    GuiLease l;
    CHECK(l.state() == GuiLease::NoAgent);
    // No lease semantics before gui_ok.
    GuiLease::Step s = l.lease_request();
    CHECK(!s.decide && s.reply.isEmpty() && l.state() == GuiLease::NoAgent);

    l.connected();
    CHECK(l.state() == GuiLease::GuiOwned && !l.lent());

    // Request, then release.
    s = l.lease_request();
    CHECK(s.decide && s.reply.isEmpty() && l.state() == GuiLease::ReleasePending && !l.lent());
    // A second request while deciding: one answer covers both.
    s = l.lease_request();
    CHECK(!s.decide && s.reply.isEmpty());
    CHECK(l.decide(true) == "released");
    CHECK(l.state() == GuiLease::McpOwned && l.lent());
    CHECK(l.decide(true).isEmpty());           // a second answer is not sent

    // The agent asks again: released at once, no question.
    s = l.lease_request();
    CHECK(!s.decide && s.reply == "released");

    // Reclaim; returns only count while one is pending.
    CHECK(l.reclaim() == "reclaim");
    CHECK(l.state() == GuiLease::ReclaimPending && l.lent());
    CHECK(l.reclaim().isEmpty());              // one reclaim at a time
    s = l.lease_returned();
    CHECK(s.unpark && l.state() == GuiLease::GuiOwned && !l.lent());
    CHECK(!l.lease_returned().unpark);         // duplicate

    // Request, then keep it (busy).
    l.lease_request();
    CHECK(l.decide(false) == "busy" && l.state() == GuiLease::GuiOwned);

    // The agent asks while our reclaim is pending: keep the analyzer (busy).
    l.lease_request();
    l.decide(true);
    l.reclaim();
    s = l.lease_request();
    CHECK(s.reply == "busy" && s.unpark && l.state() == GuiLease::GuiOwned);

    // Reclaim only from McpOwned.
    CHECK(l.reclaim().isEmpty());

    // Disconnect while lent: unpark; a new connection starts GUI-owned.
    l.lease_request();
    l.decide(true);
    s = l.disconnected();
    CHECK(s.unpark && l.state() == GuiLease::NoAgent && !l.lent());
    CHECK(l.decide(true).isEmpty());
    l.connected();
    CHECK(l.state() == GuiLease::GuiOwned);
}

int main()
{
    test_framing();
    test_messages();
    test_lease();
    if (failures) {
        std::printf("%d check(s) failed\n", failures);
        return 1;
    }
    std::printf("mcp_protocol_test: all checks passed\n");
    return 0;
}
