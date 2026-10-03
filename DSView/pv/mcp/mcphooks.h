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

// What SigSession asks before it opens a device, so the GUI leaves the analyzer
// alone while an MCP client holds it (doc/mcp-gui-protocol.md, "Device lease").
// Mac App Store edition only; without a bridge every answer allows the device.

#pragma once

#include <libsigrok.h>

namespace pv {
namespace mcp {

// True while an MCP client holds the analyzer (or the GUI waits to get it back).
bool device_lent();

// The demo device or a file opened as a device: never the analyzer.
bool is_virtual_device(ds_device_handle h);

// False if h is the analyzer and an MCP client holds it; the GUI then asks for it
// back and switches to it when it returns.
bool may_activate(ds_device_handle h);

// Records a device that SigSession created from a file.
void note_file_device(ds_device_handle h);

} // namespace mcp
} // namespace pv
