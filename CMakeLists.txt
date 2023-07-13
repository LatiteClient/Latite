# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("LatiteRecode")

# Include sub-projects.
add_subdirectory ("src")
file (GLOB SOURCES "util/Util.cpp" "util/Logger.cpp" "src/client/Latite.cpp" )

include_directories("src")

add_library(Latite SHARED ${SOURCES} "src/sdk/common/world/Minecraft.h" "src/sdk/common/world/Level.h" "src/sdk/server/ServerInstance.h" "src/api/manager/Manager.h" "src/client/module/ModuleManager.h" "src/client/module/Module.h" "src/api/manager/FeatureManager.h" "src/sdk/client/win/winrt/HidControllerWinRT.h" "src/client/command/Command.h" "src/client/script/ScriptManager.h" "src/client/script/JsScript.h")

target_precompile_headers(Latite
  PUBLIC
    src/pch.h
)

set_property(TARGET Latite PROPERTY CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD 20)