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

#include "mcpplatform.h"
#include "mcpprotocol.h"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <Security/Security.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// Channel namespace (doc/mcp-gui-protocol.md, "Identities"): "" for the App Store,
// ".tf" for TestFlight, ".dev" for development builds; set by CMake.
#ifndef LANSCAPES_MCP_SUFFIX
#define LANSCAPES_MCP_SUFFIX ""
#endif
#define MCP_TEAM_ID "BVM4W42ZKJ"
#define MCP_AGENT_ID "com.lanscapes.LogicAnalyzer.agent" LANSCAPES_MCP_SUFFIX
#define MCP_GROUP_A MCP_TEAM_ID ".com.lanscapes.la.ctl" LANSCAPES_MCP_SUFFIX
#define MCP_AGENT_APP "Logic Analyze Agent.app"

namespace pv {
namespace mcp {
namespace platform {

// The listening process must be the agent: an Apple-anchored signature with the
// agent's identifier, and this team. The kernel's audit token names the process.
static bool peer_is_agent(int fd, QString *error)
{
    audit_token_t token;
    socklen_t len = sizeof(token);
    if (getsockopt(fd, SOL_LOCAL, LOCAL_PEERTOKEN, &token, &len) != 0 || len != sizeof(token)) {
        *error = QString("cannot identify the agent (errno %1)").arg(errno);
        return false;
    }

    NSData *tokenData = [NSData dataWithBytes:&token length:sizeof(token)];
    NSDictionary *attrs = @{ (__bridge NSString *)kSecGuestAttributeAudit: tokenData };
    SecCodeRef code = NULL;
    OSStatus st = SecCodeCopyGuestWithAttributes(NULL, (__bridge CFDictionaryRef)attrs, kSecCSDefaultFlags, &code);
    if (st != errSecSuccess) {
        *error = QString("cannot identify the agent (%1)").arg((int)st);
        return false;
    }

    SecRequirementRef req = NULL;
    st = SecRequirementCreateWithString(CFSTR("anchor apple generic and identifier \"" MCP_AGENT_ID "\""),
                                        kSecCSDefaultFlags, &req);
    if (st == errSecSuccess)
        st = SecCodeCheckValidity(code, kSecCSDefaultFlags, req);
    if (req)
        CFRelease(req);

    bool team = false;
    if (st == errSecSuccess) {
        CFDictionaryRef info = NULL;
        if (SecCodeCopySigningInformation((SecStaticCodeRef)code, kSecCSSigningInformation, &info) == errSecSuccess) {
            NSString *t = ((__bridge NSDictionary *)info)[(__bridge NSString *)kSecCodeInfoTeamIdentifier];
            team = [t isEqualToString:@MCP_TEAM_ID];
            CFRelease(info);
        }
    }
    CFRelease(code);

    if (st != errSecSuccess || !team) {
        *error = QString("the process on the socket is not the Logic Analyze Agent (%1)").arg((int)st);
        return false;
    }
    return true;
}

int connect_agent(QString *error)
{
    @autoreleasepool {
        NSURL *group = [[NSFileManager defaultManager]
            containerURLForSecurityApplicationGroupIdentifier:@MCP_GROUP_A];
        if (!group || !group.isFileURL) {
            *error = "the app group container is not available";
            return -1;
        }
        const char *dir = group.fileSystemRepresentation;
        QByteArray name = QString("g%1").arg(kSecurityEpoch).toUtf8();

        int fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0) {
            *error = QString("socket: %1").arg(strerror(errno));
            return -1;
        }
        fcntl(fd, F_SETFD, FD_CLOEXEC);
        int one = 1;
        setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one));

        // sun_path holds 104 bytes: /Users/<user>/Library/Group Containers/<group A>/g1
        // fits for user names up to about 36 characters.
        struct sockaddr_un sa;
        memset(&sa, 0, sizeof(sa));
        sa.sun_family = AF_UNIX;
        if (strlen(dir) + 1 + name.size() >= sizeof(sa.sun_path)) {
            *error = "the home folder path is too long for the agent's socket";
            close(fd);
            return -1;
        }
        snprintf(sa.sun_path, sizeof(sa.sun_path), "%s/%s", dir, name.constData());
        int rc = connect(fd, (struct sockaddr *)&sa, sizeof(sa));
        if (rc != 0) {
            *error = (errno == ENOENT || errno == ECONNREFUSED)
                ? QString("the agent is not running") : QString("connect: %1").arg(strerror(errno));
            close(fd);
            return -1;
        }

        // Writes are small; never block the GUI on a stuck agent for long.
        struct timeval tv = { 2, 0 };
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
        int buf = 64 * 1024;
        setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &buf, sizeof(buf));

        if (!peer_is_agent(fd, error)) {
            close(fd);
            return -1;
        }
        return fd;
    }
}

bool open_agent(QString *error)
{
    @autoreleasepool {
        NSURL *agent = [[[NSBundle mainBundle] bundleURL]
            URLByAppendingPathComponent:@"Contents/Library/LoginItems/" MCP_AGENT_APP];
        if (![[NSFileManager defaultManager] fileExistsAtPath:agent.path]) {
            *error = "The Logic Analyze Agent is missing from this copy of the app.";
            return false;
        }
        NSWorkspaceOpenConfiguration *cfg = [NSWorkspaceOpenConfiguration configuration];
        cfg.addsToRecentItems = NO;
        [[NSWorkspace sharedWorkspace] openApplicationAtURL:agent configuration:cfg completionHandler:nil];
        return true;
    }
}

} // namespace platform
} // namespace mcp
} // namespace pv
