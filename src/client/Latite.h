#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOMINMAX
#include <Windows.h>

class Latite final {
public:
	[[nodiscard]] static Latite& get() noexcept;
	[[nodiscard]] static class ModuleManager& getModuleManager() noexcept;
	[[nodiscard]] static class CommandManager& getCommandManager() noexcept;
	[[nodiscard]] static class ClientMessageSink& getClientMessageSink() noexcept;
	[[nodiscard]] static class SettingGroup& getSettings() noexcept;
	[[nodiscard]] static class LatiteHooks& getHooks() noexcept;

	void doEject() noexcept;
	void initialize(HINSTANCE hInst);

	Latite() = default;
	~Latite() = default;
private:

	void onUnload();
	HINSTANCE dllInst;
};