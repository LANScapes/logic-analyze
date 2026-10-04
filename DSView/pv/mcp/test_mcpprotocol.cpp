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

// Hardware-free test of mcpprotocol.cpp: framing and messages.
//   cmake --build build --target mcp_protocol_test && build/mcp_protocol_test

#include "mcpprotocol.h"

#include <QJsonArray>
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

// A capture message with the given req (JSON text).
static AgentMessage capture(const char *req, const char *name = "20261003-120000-abc")
{
    QByteArray j = QByteArray("{\"v\":1,\"type\":\"capture\",\"id\":7,\"name\":\"") + name + "\",\"req\":" + req + "}";
    return parse_agent_message(QJsonDocument::fromJson(j).object());
}

static void test_framing()
{
    QByteArray f = encode_frame(hello_message());
    CHECK(f.size() > 4);
    quint32 n = (quint8(f[0]) << 24) | (quint8(f[1]) << 16) | (quint8(f[2]) << 8) | quint8(f[3]);
    CHECK(n == quint32(f.size() - 4));

    // Two frames arriving in pieces.
    QByteArray buf = f + encode_frame(capture_started_message(3));
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
    CHECK(m.value("type").toString() == "capture_started" && m.value("id").toInt() == 3);
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
    QJsonObject huge = capture_error_message(1, "failed", QString(kMaxFrameBody, 'x'));
    CHECK(encode_frame(huge).isEmpty());
}

static void test_handshake()
{
    QJsonObject h = hello_message();
    CHECK(h.size() == 6);
    CHECK(h.value("v").toInt() == 1 && h.value("type").toString() == "gui_hello");
    CHECK(h.value("proto").toInt() == 1 && h.value("epoch").toInt() == kSecurityEpoch);
    CHECK(h.value("build").toInt() == 2 && h.value("min_build").toInt() == 2);
    // Integers go out as JSON integers, never 1.0.
    CHECK(encode_frame(h).contains("\"v\":1,") || encode_frame(h).contains("\"v\":1}"));

    AgentMessage a = parse_agent_message(agent("{\"v\":1,\"type\":\"gui_ok\",\"build\":2,\"min_build\":2}"));
    CHECK(a.type == AgentMessage::Ok && a.build == 2 && a.min_build == 2);
    CHECK(parse_agent_message(agent("{\"v\":2,\"type\":\"gui_ok\",\"build\":1,\"min_build\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"gui_ok\",\"build\":1.5,\"min_build\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"gui_ok\",\"build\":1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"gui_error\",\"code\":\"version\"}")).type == AgentMessage::Error);
    CHECK(parse_agent_message(agent("{\"type\":\"devices\",\"id\":1}")).type == AgentMessage::Invalid);
    // The device lease (build 1) no longer exists.
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"lease_request\"}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"lease_returned\"}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"lease_request\",\"id\":1}")).type == AgentMessage::Invalid);
}

static void test_requests()
{
    // devices and capture_cancel need an integer id >= 0.
    AgentMessage d = parse_agent_message(agent("{\"v\":1,\"type\":\"devices\",\"id\":4}"));
    CHECK(d.type == AgentMessage::Devices && d.id == 4);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"devices\"}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"devices\",\"id\":-1}")).type == AgentMessage::Invalid);
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"devices\",\"id\":1.5}")).type == AgentMessage::Invalid);
    AgentMessage cur = parse_agent_message(agent("{\"v\":1,\"type\":\"current\",\"id\":5,\"name\":\"now-1\"}"));
    CHECK(cur.type == AgentMessage::Current && cur.id == 5 && cur.name == "now-1" && cur.req_error.isEmpty());
    cur = parse_agent_message(agent("{\"v\":1,\"type\":\"current\",\"id\":5}"));
    CHECK(cur.type == AgentMessage::Current && !cur.req_error.isEmpty());   // the agent names the files
    cur = parse_agent_message(agent("{\"v\":1,\"type\":\"current\",\"id\":5,\"name\":\"../x\"}"));
    CHECK(cur.type == AgentMessage::Current && !cur.req_error.isEmpty());
    CHECK(parse_agent_message(agent("{\"v\":1,\"type\":\"current\"}")).type == AgentMessage::Invalid);
    AgentMessage c = parse_agent_message(agent("{\"v\":1,\"type\":\"capture_cancel\",\"id\":9}"));
    CHECK(c.type == AgentMessage::CaptureCancel && c.id == 9);

    // A full request.
    AgentMessage a = capture("{\"channels\":[3,0],\"samplerate_hz\":10000000,\"samples\":4096,"
                             "\"threshold_v\":0.9,\"mode\":\"stream\",\"trigger_channel\":3,"
                             "\"trigger_edge\":\"F\",\"trigger_position_percent\":25,\"timeout_ms\":5000,"
                             "\"label\":\"ignored\"}");
    CHECK(a.type == AgentMessage::Capture && a.id == 7 && a.req_error.isEmpty());
    CHECK(a.name == "20261003-120000-abc");
    CHECK(a.req.channels.size() == 2 && a.req.channels[0] == 3 && a.req.channels[1] == 0);
    CHECK(a.req.samplerate_hz == 10000000 && a.req.samples == 4096);
    CHECK(a.req.threshold_v == 0.9 && a.req.stream == 1);
    CHECK(a.req.trigger_channel == 3 && a.req.trigger_edge == 'F' && a.req.trigger_position_percent == 25);
    CHECK(a.req.timeout_ms == 5000);

    // Defaults, as dslcap's.
    a = capture("{\"channels\":[0],\"samplerate_hz\":1000000}");
    CHECK(a.req_error.isEmpty() && a.req.samples == 1000000 && a.req.threshold_v == 1.6);
    CHECK(a.req.stream == -1 && a.req.trigger_channel == -1 && a.req.trigger_edge == 'R');
    CHECK(a.req.trigger_position_percent == 10 && a.req.timeout_ms == 30000);
    a = capture("{\"channels\":[0],\"samplerate_hz\":1000000,\"trigger_channel\":null}");
    CHECK(a.req_error.isEmpty() && a.req.trigger_channel == -1);

    // duration_s when samples is absent.
    a = capture("{\"channels\":[0],\"samplerate_hz\":1000000,\"duration_s\":0.0025}");
    CHECK(a.req_error.isEmpty() && a.req.samples == 2500);
    a = capture("{\"channels\":[0],\"samplerate_hz\":10,\"duration_s\":0.01}");
    CHECK(a.req_error.isEmpty() && a.req.samples == 1);
    a = capture("{\"channels\":[0],\"samplerate_hz\":1000000,\"samples\":10,\"duration_s\":5}");
    CHECK(a.req_error.isEmpty() && a.req.samples == 10);

    // Unusable requests are still captures, answered "unsupported".
    const char *bad[] = {
        "{\"samplerate_hz\":1000000}",
        "{\"channels\":[],\"samplerate_hz\":1000000}",
        "{\"channels\":[16],\"samplerate_hz\":1000000}",
        "{\"channels\":[1,1],\"samplerate_hz\":1000000}",
        "{\"channels\":[1.5],\"samplerate_hz\":1000000}",
        "{\"channels\":[0]}",
        "{\"channels\":[0],\"samplerate_hz\":0}",
        "{\"channels\":[0],\"samplerate_hz\":1000000,\"samples\":0}",
        "{\"channels\":[0],\"samplerate_hz\":1000000,\"duration_s\":0}",
        "{\"channels\":[0],\"samplerate_hz\":1000000,\"threshold_v\":5.5}",
        "{\"channels\":[0],\"samplerate_hz\":1000000,\"mode\":\"fast\"}",
        "{\"channels\":[0],\"samplerate_hz\":1000000,\"trigger_channel\":1}",
        "{\"channels\":[0],\"samplerate_hz\":1000000,\"trigger_edge\":\"X\"}",
        "{\"channels\":[0],\"samplerate_hz\":1000000,\"trigger_edge\":\"RF\"}",
        "{\"channels\":[0],\"samplerate_hz\":1000000,\"trigger_position_percent\":101}",
        "{\"channels\":[0],\"samplerate_hz\":1000000,\"timeout_ms\":0}",
        "[1]",
    };
    for (const char *r : bad) {
        a = capture(r);
        CHECK(a.type == AgentMessage::Capture && !a.req_error.isEmpty());
        if (a.req_error.isEmpty())
            std::printf("  accepted: %s\n", r);
    }
    // Names: a plain file base name.
    CHECK(capture("{\"channels\":[0],\"samplerate_hz\":1000000}", "../x").req_error.size() > 0);
    CHECK(capture("{\"channels\":[0],\"samplerate_hz\":1000000}", "-x").req_error.size() > 0);
    CHECK(capture("{\"channels\":[0],\"samplerate_hz\":1000000}", "").req_error.size() > 0);
    CHECK(capture("{\"channels\":[0],\"samplerate_hz\":1000000}", "a.b").req_error.size() > 0);
    CHECK(capture("{\"channels\":[0],\"samplerate_hz\":1000000}", "_ok-1").req_error.isEmpty());
}

static void test_replies()
{
    QJsonObject d = devices_ok_message(4, {"Demo Device", "DSLogic Plus"}, "DSLogic Plus");
    CHECK(d.value("type").toString() == "devices_ok" && d.value("id").toInt() == 4);
    CHECK(d.value("devices").toArray().size() == 2 && d.value("selected").toString() == "DSLogic Plus");

    QJsonObject s = capture_started_message(7);
    CHECK(s.size() == 3 && s.value("type").toString() == "capture_started" && s.value("id").toInt() == 7);

    QJsonObject meta{{"samples", 4096}, {"stopped_by_user", true}};
    QJsonObject done = capture_done_message(7, "n", meta);
    CHECK(done.value("type").toString() == "capture_done" && done.value("name").toString() == "n");
    CHECK(done.value("meta").toObject() == meta);

    QJsonObject cur = current_ok_message(8, "now-1", meta);
    CHECK(cur.value("type").toString() == "current_ok" && cur.value("id").toInt() == 8);
    CHECK(cur.value("name").toString() == "now-1" && cur.value("meta").toObject() == meta);

    QJsonObject e = capture_error_message(7, "busy", "later");
    CHECK(e.value("type").toString() == "capture_error" && e.value("code").toString() == "busy");
    CHECK(e.value("message").toString() == "later" && e.value("id").toInt() == 7);
}

int main()
{
    test_framing();
    test_handshake();
    test_requests();
    test_replies();
    if (failures) {
        std::printf("%d check(s) failed\n", failures);
        return 1;
    }
    std::printf("mcp_protocol_test: all checks passed\n");
    return 0;
}
