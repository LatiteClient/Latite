cmake_minimum_required (VERSION 3.22)

project(Latite)

if(NOT MSVC)
	message(FATAL_ERROR "Compiler unsupported, must use MSVC")
endif()

# add all sources in src/* and deps/*
file(GLOB_RECURSE Latite_SRC CONFIGURE_DEPENDS
	"src/*.h"
	"src/*.cpp"

	"deps/*.h"
	"deps/*.cpp"
)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# MSVC library release ABI compatibility with game
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

add_library(Latite SHARED ${Latite_SRC})

# C++ standard
set_property(TARGET Latite PROPERTY CXX_STANDARD 20)

# disable RTTI and set warning level
add_definitions(/GR- /D_HAS_STATIC_RTTI=0 /W4 /MT)
add_compile_definitions(_UNICODE UNICODE)

# Link libraries
target_link_libraries(Latite PRIVATE
	d3d11
	d3d12
	d2d1
	dwrite
	SHCore
	Dbghelp
	dxguid
	dxgi
	dwrite
	winmm
)

target_include_directories(Latite PRIVATE
	src
	deps
)

add_subdirectory(src)
add_subdirectory(deps)