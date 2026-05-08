include(get_cpm.cmake)

CPMAddPackage(NAME glm VERSION 1.0.1 URL https://ghfast.top/https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip)
CPMAddPackage(NAME minhook VERSION 1.3.4 URL https://ghfast.top/https://github.com/TsudaKageyu/minhook/archive/refs/tags/v1.3.4.zip)
CPMAddPackage(NAME mnemosyne URL https://ghfast.top/https://github.com/brampedgex/mnemosyne/archive/48078d302fa3ba9e22b80ca69eadf27811caf361.zip OPTIONS "BUILD_TESTING OFF")
CPMAddPackage(NAME nlohmann_json VERSION 3.12.0 URL https://ghfast.top/https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.zip)
CPMAddPackage(NAME libhat VERSION 0.6.0 URL https://ghfast.top/https://github.com/BasedInc/libhat/archive/refs/tags/v0.6.0.zip)

target_include_directories(Latite PRIVATE
	"include"
)

target_link_libraries(Latite PRIVATE
	minhook
	glm::glm
	mnemosyne
	nlohmann_json
	libhat::libhat
)
