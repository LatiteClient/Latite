#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOMINMAX
#include <Windows.h>
#include <string_view>
#include "api/eventing/Listenable.h"

class Latite final : public Listener {
public:
	static Latite& get() noexcept;
	[[nodiscard]] static class ModuleManager& getModuleManager() noexcept;
	[[nodiscard]] static class CommandManager& getCommandManager() noexcept;
	[[nodiscard]] static class ConfigManager& getConfigManager() noexcept;
	[[nodiscard]] static class ClientMessageSink& getClientMessageSink() noexcept;
	[[nodiscard]] static class SettingGroup& getSettings() noexcept;
	[[nodiscard]] static class LatiteHooks& getHooks() noexcept;
	[[nodiscard]] static class Eventing& getEventing() noexcept;
	[[nodiscard]] static class Renderer& getRenderer() noexcept;

	void queueEject() noexcept;
	void initialize(HINSTANCE hInst);

	void onUpdate(class Event& ev);
	void loadConfig(class SettingGroup& resolvedGroup);

	Latite() = default;
	~Latite() = default;

	static constexpr std::string_view version = "v2.0.0";
	HINSTANCE dllInst;
private:

	void doEject() noexcept;
	void threadsafeInit();

	bool shouldEject = false;
	bool hasInit = false;
};