include(get_cpm.cmake)

CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:TsudaKageyu/minhook@1.3.4")
CPMAddPackage("gh:brampedgex/mnemosyne#48078d302fa3ba9e22b80ca69eadf27811caf361")
CPMAddPackage("gh:nlohmann/json@3.12.0")
CPMAddPackage("gh:BasedInc/libhat@0.6.0")
CPMAddPackage("gh:skypjack/entt#fe8d7d78c4823e8a66a050bf86f5c6318cf76ce7")
CPMAddPackage(
    NAME SDL
    VERSION 3.4.10
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.10/SDL3-3.4.10.tar.gz
    URL_HASH SHA256=12b34280415ec8418c864408b93d008a20a6530687ee613d60bfbd20411f2785
    OPTIONS
        "SDL_SHARED OFF"
        "SDL_STATIC ON"
        "SDL_TEST_LIBRARY OFF"
        "SDL_TESTS OFF"
        "SDL_EXAMPLES OFF"
        "SDL_INSTALL OFF"
        "SDL_UNINSTALL OFF"
        "SDL_AUDIO OFF"
        "SDL_VIDEO OFF"
        "SDL_GPU OFF"
        "SDL_RENDER OFF"
        "SDL_CAMERA OFF"
        "SDL_HAPTIC OFF"
        "SDL_POWER OFF"
        # GameInput keeps standalone Windows gyro and accelerometer data paired.
        "SDL_SENSOR OFF"
        "SDL_DIALOG OFF"
        "SDL_TRAY OFF"
        "SDL_HIDAPI ON"
        "SDL_HIDAPI_JOYSTICK ON"
        "SDL_VIRTUAL_JOYSTICK OFF"
)
CPMAddPackage(
    NAME MicrosoftGameInput
    VERSION 3.5.262
    URL https://www.nuget.org/api/v2/package/Microsoft.GameInput/3.5.262
    DOWNLOAD_ONLY YES
)

target_include_directories(Latite PRIVATE
	"include"
    "${MicrosoftGameInput_SOURCE_DIR}/native/include"
)

target_sources(Latite PRIVATE
    "${MicrosoftGameInput_SOURCE_DIR}/native/src/GameInput.cpp"
)

target_link_libraries(Latite PRIVATE
	minhook
	glm::glm
	mnemosyne
	nlohmann_json
	libhat::libhat
	EnTT::EnTT
    SDL3::SDL3-static
)
