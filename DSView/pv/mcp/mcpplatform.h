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

// The Apple APIs the agent connection needs (mcpplatform.mm).
// Mac App Store edition only.

#pragma once

#include <QString>

namespace pv {
namespace mcp {
namespace platform {

// Connects to the agent's GUI socket g<epoch> in app group A and checks that the
// listening process is the agent (Apple-anchored signature with the agent's
// identifier and this team). Returns the connected descriptor, or -1 and a reason.
int connect_agent(QString *error);

// Starts the nested agent app (or brings up its menu-bar item if it is running).
// The agent serves only while this GUI is connected and exits when it disconnects.
bool open_agent(QString *error);

} // namespace platform
} // namespace mcp
} // namespace pv
