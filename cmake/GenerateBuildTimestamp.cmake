if (NOT DEFINED OUTPUT_FILE OR OUTPUT_FILE STREQUAL "")
    message(FATAL_ERROR "OUTPUT_FILE is required")
endif ()

if (DEFINED LATITE_BUILD_TIMESTAMP AND NOT LATITE_BUILD_TIMESTAMP STREQUAL "")
    set(build_timestamp "${LATITE_BUILD_TIMESTAMP}")
else ()
    string(TIMESTAMP build_timestamp "%Y-%m-%d_%H-%M-%SZ" UTC)
endif ()

if (NOT build_timestamp MATCHES
        "^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]_[0-9][0-9]-[0-9][0-9]-[0-9][0-9]Z$")
    message(FATAL_ERROR
            "Build timestamp must use YYYY-MM-DD_HH-mm-ssZ UTC+0 format; got '${build_timestamp}'")
endif ()

get_filename_component(output_directory "${OUTPUT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${output_directory}")
file(WRITE "${OUTPUT_FILE}"
"#include \"client/BuildTimestamp.h\"\n\
\n\
const char* LatiteBuild::getTimestamp() noexcept {\n\
    return \"${build_timestamp}\";\n\
}\n")
