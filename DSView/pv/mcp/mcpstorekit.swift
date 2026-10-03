// This file is part of the Logic Analyze project (Mac App Store edition).
//
// Copyright (C) 2026 Lanscapes
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Purchase evidence for the MCP agent (doc/mcp-gui-protocol.md, "evidence").
// AppTransaction and AppStore.deviceVerificationID are Swift-only StoreKit APIs,
// so this one function is Swift; the GUI calls it from C++. It does no
// verification: the agent verifies the JWS.
import Foundation
import StoreKit

public typealias LAEvidenceCallback = @convention(c) (
    UnsafeMutableRawPointer?, UnsafePointer<CChar>?, UnsafePointer<CChar>?, UnsafePointer<CChar>?) -> Void

@_cdecl("la_mcp_fetch_app_transaction")
public func laMcpFetchAppTransaction(_ refresh: Int32, _ ctx: UnsafeMutableRawPointer?, _ cb: LAEvidenceCallback) {
    nonisolated(unsafe) let ctx = ctx
    Task.detached {
        do {
            // refresh() may show a sign-in prompt; the GUI calls it only from a button.
            let result = refresh != 0 ? try await AppTransaction.refresh() : try await AppTransaction.shared
            guard let dvid = AppStore.deviceVerificationID?.uuidString else {
                cb(ctx, nil, nil, "This Mac has no App Store device verification ID.")
                return
            }
            result.jwsRepresentation.withCString { jws in
                dvid.withCString { d in cb(ctx, jws, d, nil) }
            }
        } catch {
            "\(error.localizedDescription)".withCString { e in cb(ctx, nil, nil, e) }
        }
    }
}
