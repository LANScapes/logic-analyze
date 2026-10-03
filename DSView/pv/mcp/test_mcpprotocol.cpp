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

static const QString E1 = "0123456789abcdef0123456789abcdef";
static const QString E2 = "fedcba9876543210fedcba9876543210";
static const QString N1 = "11111111111111111111111111111111";
static const QString N2 = "22222222222222222222222222222222";

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
    QByteArray buf = f + encode_frame(evidence_message("a.b.c", "51E35FBA-0000-4000-8000-000000000000"));
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
    CHECK(m.value("type").toString() == "evidence");
    CHECK(m.value("jws").toString() == "a.b.c");
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

    // An evidence frame with a 16 KiB JWS fits; a 32 KiB one is refused.
    CHECK(!encode_frame(evidence_message(QString(kMaxJwsBytes, 'x'), "u")).isEmpty());
    CHECK(encode_frame(evidence_message(QString(kMaxFrameBody, 'x'), "u")).isEmpty());
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

    QJsonObject e = evidence_message("j", "d");
    CHECK(e.size() == 4 && e.value("device_verification_id").toString() == "d");

    QJsonObject l = lease_message("released", N1, E1, 3);
    CHECK(l.size() == 7);
    CHECK(l.value("device").toString() == "default" && l.value("device_generation").toInt() == 3);
    CHECK(encode_frame(l).contains("\"device_generation\":3"));

    AgentMessage a = parse_agent_message(agent("{\"v\":1,\"type\":\"gui_ok\",\"boot_epoch\":\"0123456789abcdef0123456789abcdef\",\"build\":1,\"min_build\":1}"));
    CHECK(a.type == AgentMessage::Ok && a.boot_epoch == E1 && a.build == 1);
    CHECK(parse_agent_message(agent("{\"v\":2,\"type\":\"gui_ok\",\"boot_epoch\":\"0123456789abcdef0123456789abcdef\",\"build\":1,\"min_build\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"gui_ok\",\"boot_epoch\":\"0123456789ABCDEF0123456789abcdef\",\"build\":1,\"min_build\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"gui_ok\",\"boot_epoch\":\"0123456789abcdef0123456789abcdef\",\"build\":1.5,\"min_build\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"gui_error\",\"code\":\"version\"}")).type == AgentMessage::Error);

    a = parse_agent_message(agent("{\"v\":1,\"type\":\"evidence_result\",\"result\":\"granted\",\"expires_at\":1791000000000}"));
    CHECK(a.type == AgentMessage::EvidenceResult && a.result == "granted" && a.expires_at == 1791000000000LL);
    a = parse_agent_message(agent("{\"v\":1,\"type\":\"evidence_result\",\"result\":\"not_newer\",\"expires_at\":null}"));
    CHECK(a.type == AgentMessage::EvidenceResult && a.expires_at == -1);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"evidence_result\",\"result\":\"maybe\",\"expires_at\":null}")).type == AgentMessage::Invalid);

    a = parse_agent_message(agent("{\"v\":1,\"type\":\"lease_request\",\"nonce\":\"11111111111111111111111111111111\",\"boot_epoch\":\"0123456789abcdef0123456789abcdef\",\"device\":\"default\",\"device_generation\":2}"));
    CHECK(a.type == AgentMessage::LeaseRequest && a.nonce == N1 && a.device_generation == 2);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"lease_returned\",\"nonce\":\"1111\",\"boot_epoch\":\"0123456789abcdef0123456789abcdef\",\"device\":\"default\",\"device_generation\":2}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"lease_returned\",\"nonce\":\"11111111111111111111111111111111\",\"boot_epoch\":\"0123456789abcdef0123456789abcdef\",\"device\":\"other\",\"device_generation\":2}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"status\"}")).type == AgentMessage::Invalid);

    QString n = new_nonce();
    CHECK(is_hex32(n) && n != new_nonce());
}

static void test_lease()
{
    GuiLease l;
    CHECK(l.state() == GuiLease::NoAgent);
    // No lease semantics before gui_ok.
    CHECK(!l.lease_request(N1, E1, 1).decide);

    l.connected(E1);
    CHECK(l.state() == GuiLease::GuiOwned && !l.lent());

    // Wrong boot epoch: ignored.
    GuiLease::Step s = l.lease_request(N1, E2, 1);
    CHECK(!s.decide && !s.reply.send && l.state() == GuiLease::GuiOwned);

    // Request, then release.
    s = l.lease_request(N1, E1, 1);
    CHECK(s.decide && !s.reply.send && l.state() == GuiLease::ReleasePending && !l.lent());
    GuiLease::Reply r = l.decide(true);
    CHECK(r.send && r.op == "released" && r.nonce == N1 && r.device_generation == 1);
    CHECK(l.state() == GuiLease::McpOwned && l.lent());
    CHECK(!l.decide(true).send);               // a second answer is not sent

    // The agent asks again (it lost the answer): released at once, no question.
    s = l.lease_request(N2, E1, 1);
    CHECK(!s.decide && s.reply.send && s.reply.op == "released" && s.reply.nonce == N2);

    // Reclaim, with stale and foreign returns ignored.
    r = l.reclaim(N1);
    CHECK(r.send && r.op == "reclaim" && r.nonce == N1 && r.device_generation == 1);
    CHECK(l.state() == GuiLease::ReclaimPending && l.lent());
    CHECK(!l.reclaim(N2).send);                // one reclaim at a time
    CHECK(!l.lease_returned(N2, E1, 1).unpark);
    CHECK(!l.lease_returned(N1, E2, 1).unpark);
    CHECK(!l.lease_returned(N1, E1, 2).unpark);
    s = l.lease_returned(N1, E1, 1);
    CHECK(s.unpark && l.state() == GuiLease::GuiOwned && !l.lent());
    CHECK(!l.lease_returned(N1, E1, 1).unpark); // duplicate

    // Request, then keep it (busy).
    l.lease_request(N1, E1, 1);
    r = l.decide(false);
    CHECK(r.send && r.op == "busy" && r.nonce == N1 && l.state() == GuiLease::GuiOwned);

    // A newer request while deciding: the answer carries the newer nonce.
    CHECK(l.lease_request(N1, E1, 1).decide);
    s = l.lease_request(N2, E1, 1);
    CHECK(!s.decide && !s.reply.send);
    r = l.decide(true);
    CHECK(r.nonce == N2);

    // The agent asks while our reclaim is pending: keep the analyzer (busy).
    l.reclaim(N1);
    s = l.lease_request(N2, E1, 1);
    CHECK(s.reply.send && s.reply.op == "busy" && s.unpark && l.state() == GuiLease::GuiOwned);

    // Reclaim only from McpOwned.
    CHECK(!l.reclaim(N1).send);

    // Disconnect while lent: unpark; a new connection starts GUI-owned.
    l.lease_request(N1, E1, 1);
    l.decide(true);
    s = l.disconnected();
    CHECK(s.unpark && l.state() == GuiLease::NoAgent && !l.lent());
    CHECK(!l.decide(true).send);
    s = l.connected(E2);
    CHECK(!s.unpark && l.state() == GuiLease::GuiOwned && l.boot_epoch() == E2);
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
