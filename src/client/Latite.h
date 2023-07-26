#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <string_view>
#include "api/eventing/Listenable.h"
#include "api/feature/setting/Setting.h"

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
	[[nodiscard]] static class ScreenManager& getScreenManager() noexcept;
	[[nodiscard]] static class Assets& getAssets() noexcept;
	[[nodiscard]] static class ScriptManager& getScriptManager() noexcept;

	void queueEject() noexcept;
	void initialize(HINSTANCE hInst);

	void onUpdate(class Event& ev);
	void onKey(class Event& ev);
	void onRendererInit(class Event& ev);
	void onFocusLost(class Event& ev);
	void onSuspended(class Event& ev);
	void loadConfig(class SettingGroup& resolvedGroup);

	Latite() = default;
	~Latite() = default;

	static constexpr std::string_view version = "v2.0.0";
	HINSTANCE dllInst;
private:
	Setting::Value menuKey = KeyValue('M');

	void doEject() noexcept;
	void threadsafeInit();
	void initSettings();

	bool shouldEject = false;
	bool hasInit = false;
};