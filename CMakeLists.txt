cmake_minimum_required(VERSION 3.22)

project(Latite)

if (NOT MSVC)
    message(FATAL_ERROR "Compiler unsupported, must use MSVC")
endif ()

# Add all sources in src/* and deps/*
file(GLOB_RECURSE Latite_SRC CONFIGURE_DEPENDS
        "src/*.h"
        "src/*.cpp"

        "deps/*.h"
        "deps/*.cpp"
)

# Enable Hot Reload for MSVC compilers if supported
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif ()

# MSVC library release ABI compatibility with game
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

# Assets

function(ADD_RESOURCES out_var)
    set(result)
    foreach (in_f ${ARGN})
        file(RELATIVE_PATH src_f ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f})
        set(out_f "${PROJECT_BINARY_DIR}/${in_f}.o")
        add_custom_command(OUTPUT ${out_f}
                COMMAND ld.exe -r -b binary -o ${out_f} ${src_f}
                DEPENDS ${in_f}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Building object ${out_f}"
                VERBATIM
        )
        list(APPEND result ${out_f})
    endforeach()
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

# Add all files in assets/ as resources

file(GLOB_RECURSE RESOURCE_FILES RELATIVE ${CMAKE_CURRENT_LIST_DIR} CONFIGURE_DEPENDS "assets/*")
add_resources(EMBEDDED_RESOURCES ${RESOURCE_FILES})

add_library(Latite SHARED ${Latite_SRC} ${EMBEDDED_RESOURCES})

# C++ standard
set_property(TARGET Latite PROPERTY CXX_STANDARD 20)

# Disable RTTI and set warning level
add_definitions(/GR- /D_HAS_STATIC_RTTI=0 /W4 /MT)
add_compile_definitions(_UNICODE UNICODE)

# Link libraries
target_link_libraries(Latite PRIVATE
        # WinRT
        WindowsApp.lib
        Shlwapi.lib

        # Needed for getting app version
        Version.lib

        # Needed for DirectX hooking and Direct2D rendering
        d3d11
        d3d12
        dxguid
        dxgi

        # UI rendering
        d2d1

        # Text rendering
        dwrite

        # ImageNtHeader for mnemosyne
        Dbghelp

        # PlaySound for scripting
        winmm
)

target_include_directories(Latite PRIVATE
        src
        deps
)

add_subdirectory(src)
add_subdirectory(deps)