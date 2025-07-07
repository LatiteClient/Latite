target_include_directories(Latite PRIVATE
	"include"
	"include/mnemosyne"
)

target_link_libraries(Latite PRIVATE
	"${CMAKE_SOURCE_DIR}/deps/lib/MinHook.lib"
)