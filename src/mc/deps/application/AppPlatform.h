#pragma once
namespace SDK {
	class AppPlatform {
	private:
		[[maybe_unused]] char pad[0x20];
	public:
		class MinecraftGame* game;

		class AppPlatform* get();
	};
}