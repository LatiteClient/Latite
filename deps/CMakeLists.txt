include(cmake/get_cpm.cmake)

CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:TsudaKageyu/minhook@1.3.4")
CPMAddPackage("gh:brampedgex/mnemosyne#48078d302fa3ba9e22b80ca69eadf27811caf361")
CPMAddPackage("gh:nlohmann/json@3.12.0")

target_include_directories(Latite PRIVATE
	"include"
)

target_link_libraries(Latite PRIVATE
	minhook
	glm::glm
	mnemosyne
	nlohmann_json
)
