# Mac App Store edition: the GUI side of the MCP agent channel g<epoch>
# (doc/mcp-gui-protocol.md). Included by CMakeLists.txt only when
# LANSCAPES_APPSTORE is ON; the default and brand-only builds never see it.

if(NOT APPLE)
	message(FATAL_ERROR "LANSCAPES_APPSTORE builds only on macOS")
endif()

# The agent namespace this GUI talks to (agent identifier and app group suffix).
set(LANSCAPES_MCP_CHANNEL "production" CACHE STRING "MCP agent channel: production, testflight or development")
if(LANSCAPES_MCP_CHANNEL STREQUAL "production")
	set(MCP_SUFFIX "")
elseif(LANSCAPES_MCP_CHANNEL STREQUAL "testflight")
	set(MCP_SUFFIX ".tf")
elseif(LANSCAPES_MCP_CHANNEL STREQUAL "development")
	set(MCP_SUFFIX ".dev")
else()
	message(FATAL_ERROR "LANSCAPES_MCP_CHANNEL must be production, testflight or development")
endif()

enable_language(OBJCXX)
set(MCP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DSView/pv/mcp)

if(Qt6Core_FOUND)
	qt6_wrap_cpp(MCP_MOC ${MCP_DIR}/mcpbridge.h)
	set(MCP_QTCORE Qt6::Core)
else()
	qt5_wrap_cpp(MCP_MOC ${MCP_DIR}/mcpbridge.h)
	set(MCP_QTCORE Qt5::Core)
endif()

set_source_files_properties(${MCP_DIR}/mcpplatform.mm PROPERTIES COMPILE_FLAGS "-fobjc-arc")

# Also added to lang_ui_check (CMakeLists.txt), which checks the MCP pane.
function(add_mcp_sources target)
	target_sources(${target} PRIVATE
		${MCP_DIR}/mcpprotocol.cpp
		${MCP_DIR}/mcpbridge.cpp
		${MCP_DIR}/mcpplatform.mm
		${MCP_MOC})
	target_compile_definitions(${target} PRIVATE LANSCAPES_MCP_SUFFIX="${MCP_SUFFIX}")
	target_link_libraries(${target} "-framework AppKit" "-framework Security")
endfunction()
add_mcp_sources(${PROJECT_NAME})

# Hardware-free test of the framing, messages and lease state machine:
#   cmake --build build --target mcp_protocol_test && build/mcp_protocol_test
add_executable(mcp_protocol_test EXCLUDE_FROM_ALL ${MCP_DIR}/test_mcpprotocol.cpp ${MCP_DIR}/mcpprotocol.cpp)
target_link_libraries(mcp_protocol_test ${MCP_QTCORE})
set_target_properties(mcp_protocol_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
