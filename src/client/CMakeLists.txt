target_sources(Latite PRIVATE
	src/client/Latite.h
	src/client/Latite.cpp)

add_subdirectory(command)
add_subdirectory(hook)
add_subdirectory(misc)
add_subdirectory(module)
add_subdirectory(script)