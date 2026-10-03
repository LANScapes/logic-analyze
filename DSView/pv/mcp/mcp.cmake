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

# AppTransaction and AppStore.deviceVerificationID are Swift-only, so one small
# Swift file is compiled to an object here (the Makefile generator has no Swift).
execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE MCP_SDK OUTPUT_STRIP_TRAILING_WHITESPACE
	RESULT_VARIABLE mcp_rc)
if(NOT mcp_rc EQUAL 0)
	message(FATAL_ERROR "xcrun --show-sdk-path failed")
endif()
if(CMAKE_OSX_DEPLOYMENT_TARGET)
	set(MCP_MIN_MACOS ${CMAKE_OSX_DEPLOYMENT_TARGET})
else()
	set(MCP_MIN_MACOS 26.0)
endif()
set(MCP_SWIFT_OBJ ${CMAKE_CURRENT_BINARY_DIR}/mcpstorekit.o)
add_custom_command(OUTPUT ${MCP_SWIFT_OBJ}
	COMMAND xcrun swiftc -parse-as-library -O -swift-version 5 -module-name LogicAnalyzeStoreKit
		-target ${CMAKE_HOST_SYSTEM_PROCESSOR}-apple-macos${MCP_MIN_MACOS} -sdk ${MCP_SDK}
		-emit-object -o ${MCP_SWIFT_OBJ} ${MCP_DIR}/mcpstorekit.swift
	DEPENDS ${MCP_DIR}/mcpstorekit.swift
	COMMENT "Compiling mcpstorekit.swift")
set_source_files_properties(${MCP_SWIFT_OBJ} PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
set_source_files_properties(${MCP_DIR}/mcpplatform.mm PROPERTIES COMPILE_FLAGS "-fobjc-arc")

target_sources(${PROJECT_NAME} PRIVATE
	${MCP_DIR}/mcpprotocol.cpp
	${MCP_DIR}/mcpbridge.cpp
	${MCP_DIR}/mcpplatform.mm
	${MCP_MOC}
	${MCP_SWIFT_OBJ})
target_compile_definitions(${PROJECT_NAME} PRIVATE LANSCAPES_MCP_SUFFIX="${MCP_SUFFIX}")
# The Swift runtime and its overlays come from the OS (/usr/lib/swift).
target_link_directories(${PROJECT_NAME} PRIVATE ${MCP_SDK}/usr/lib/swift)
target_link_libraries(${PROJECT_NAME} "-framework AppKit" "-framework Security"
	"-framework StoreKit" "-Wl,-rpath,/usr/lib/swift")

# Hardware-free test of the framing, messages and lease state machine:
#   cmake --build build --target mcp_protocol_test && build/mcp_protocol_test
add_executable(mcp_protocol_test EXCLUDE_FROM_ALL ${MCP_DIR}/test_mcpprotocol.cpp ${MCP_DIR}/mcpprotocol.cpp)
target_link_libraries(mcp_protocol_test ${MCP_QTCORE})
set_target_properties(mcp_protocol_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
