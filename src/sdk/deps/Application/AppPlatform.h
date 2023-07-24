#pragma once
namespace sdk {
	class AppPlatform {
	private:
		[[maybe_unused]] char pad[0x20];
	public:
		class MinecraftGame* game;
	};
}