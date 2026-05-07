cmake_minimum_required(VERSION 3.22)

project(Latite)

# Allow Nightly as a valid configuration. Public builds intentionally emit
# private PDBs so crash dumps can be symbolicated later without shipping those
# PDBs to users.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel;Nightly" CACHE STRING "" FORCE)

set(CMAKE_C_FLAGS_NIGHTLY "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_NIGHTLY "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_NIGHTLY "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_NIGHTLY "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)

if (NOT MSVC)
    message(FATAL_ERROR "Compiler unsupported, must use MSVC")
endif()

# Enable Hot Reload / debug info format for supported MSVC generators
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug>:EditAndContinue>$<$<CONFIG:Release,RelWithDebInfo,Nightly>:ProgramDatabase>")
endif ()

# MSVC runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

# Add all sources in src/* and deps/*
file(GLOB_RECURSE Latite_SRC CONFIGURE_DEPENDS
        "src/*.h"
        "src/*.cpp"
        "deps/*.h"
        "deps/*.cpp"
)

# Assets
function(ADD_RESOURCES out_var)
    set(result)
    foreach(in_f ${ARGN})
        file(RELATIVE_PATH src_f ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f})
        set(out_f "${PROJECT_BINARY_DIR}/${in_f}.o")

        add_custom_command(
                OUTPUT ${out_f}
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
file(GLOB_RECURSE RESOURCE_FILES
        RELATIVE ${CMAKE_CURRENT_LIST_DIR}
        CONFIGURE_DEPENDS
        "assets/*"
)

add_resources(EMBEDDED_RESOURCES ${RESOURCE_FILES})

add_library(Latite SHARED
        ${Latite_SRC}
        ${EMBEDDED_RESOURCES}
)

# Final artifact names. Keep these in CMake so the DLL CodeView record and
# linker-generated PDB name agree, **do not rename the DLL/PDB after linking!!**
set_target_properties(Latite PROPERTIES
        OUTPUT_NAME_RELEASE "Latite"
        PDB_NAME_RELEASE "Latite"
        OUTPUT_NAME_RELWITHDEBINFO "Latite"
        PDB_NAME_RELWITHDEBINFO "Latite"
        OUTPUT_NAME_DEBUG "LatiteDebug"
        PDB_NAME_DEBUG "LatiteDebug"
        OUTPUT_NAME_NIGHTLY "LatiteNightly"
        PDB_NAME_NIGHTLY "LatiteNightly"
)

# C++ standard
set_property(TARGET Latite PROPERTY CXX_STANDARD 23)
set_property(TARGET Latite PROPERTY CXX_STANDARD_REQUIRED ON)

# Compile definitions
target_compile_definitions(Latite PRIVATE
        _UNICODE
        UNICODE
        _HAS_STATIC_RTTI=0

        $<$<CONFIG:Debug>:LATITE_DEBUG>
        $<$<CONFIG:Debug>:LATITE_CRASH_REPORTING>
        $<$<CONFIG:Release>:LATITE_RELEASE>
        $<$<CONFIG:Release>:LATITE_CRASH_REPORTING>
        $<$<CONFIG:RelWithDebInfo>:LATITE_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:LATITE_CRASH_REPORTING>
        $<$<CONFIG:MinSizeRel>:LATITE_RELEASE>
        $<$<CONFIG:Nightly>:LATITE_NIGHTLY>
        $<$<CONFIG:Nightly>:LATITE_CRASH_REPORTING>
)

# Compile options
target_compile_options(Latite PRIVATE
        /EHsc
        /GR-
        /W4
)

target_link_options(Latite PRIVATE
        $<$<CONFIG:Release>:/DEBUG:FULL>
)

target_include_directories(Latite PRIVATE
        src
        deps
)

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

add_subdirectory(src)
add_subdirectory(deps)
