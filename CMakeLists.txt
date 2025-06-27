cmake_minimum_required (VERSION 3.22)

project(Latite)

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

add_library(Latite SHARED ${Latite_SRC})

# C++20 standard
set_property(TARGET Latite PROPERTY CXX_STANDARD 20)

# disable RTTI and set warning level
add_definitions(/GR- /D_HAS_STATIC_RTTI=0 /W4)

# MSVC library release ABI compatibility with game
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MD")

target_include_directories(Latite PRIVATE
	src
	deps
	"deps/include"
)

add_subdirectory(src)
add_subdirectory(deps)