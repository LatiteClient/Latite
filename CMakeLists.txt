cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("LatiteRecode")

# Include sub-projects.

file (GLOB SOURCES "src/util/Util.cpp" "src/util/Logger.cpp" "src/client/Latite.cpp" )

include_directories("src" "." "deps/include")

add_library(Latite SHARED ${SOURCES} "src/sdk/common/world/Minecraft.h" "src/sdk/common/world/level/Level.h" "src/sdk/common/server/ServerInstance.h" "src/api/manager/Manager.h" "src/client/module/ModuleManager.h" "src/client/module/Module.h" "src/api/manager/FeatureManager.h" "src/sdk/common/client/win/winrt/HidControllerWinRT.h" "src/client/command/Command.h" "src/client/script/ScriptManager.h" "src/client/script/JsScript.h" "src/client/hook/Hook.h" "src/client/hook/Hook.cpp" "src/client/hook/impl/GeneralHooks.h" "src/client/misc/ClientMessageSink.h" "src/client/command/CommandManager.h" "src/api/memory/memory.h")

add_subdirectory ("src")
add_subdirectory ("deps")

target_link_libraries(Latite PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/lib/MinHook.lib")
# todo: add dxguid.lib, d2d1.lib, and stuff
include_directories("src/api")

target_precompile_headers(Latite
  PUBLIC
    src/pch.h
)

set_property(TARGET Latite PROPERTY CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD 20)
