#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <string_view>
#include "api/eventing/Listenable.h"
#include "api/feature/setting/Setting.h"
#include <optional>
#include <winrt/windows.foundation.h>
#include "ui/TextBox.h"
#include "misc/Timings.h"

class Latite final : public Listener {
public:
	[[nodiscard]] static Latite& get() noexcept;

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
	[[nodiscard]] static class Keyboard& getKeyboard() noexcept;

	[[nodiscard]] Timings& getTimings() noexcept { return timings; }

	void queueEject() noexcept;
	void initialize(HINSTANCE hInst);

	void onUpdate(class Event& ev);
	void onKey(class Event& ev);
	void onClick(class Event& ev);
	void onChar(class Event& ev);
	void onRendererInit(class Event& ev);
	void onFocusLost(class Event& ev);
	void onSuspended(class Event& ev);
	void loadConfig(class SettingGroup& resolvedGroup);
	void initAsset(int resource, std::wstring const& filename);
	std::string getTextAsset(int resource);
	winrt::Windows::Foundation::IAsyncAction downloadExtraAssets();

	Latite() = default;
	~Latite() = default;

	static constexpr std::string_view version = "v2.0.0";
	HINSTANCE dllInst;

	std::optional<float> getMenuBlur();

	void addTextBox(ui::TextBox* box) {
		textBoxes.push_back(box);
	}

	void removeTextBox(ui::TextBox* box) {
		for (auto it = textBoxes.begin(); it != textBoxes.end(); ++it) {
			if (*it == box) {
				textBoxes.erase(it);
				break;
			}
		}
	}

	[[nodiscard]] KeyValue getMenuKey() {
		return std::get<KeyValue>(menuKey);
	}

	[[nodiscard]] bool shouldForceDX11() {
		return std::get<BoolValue>(useDX11);
	}
private:
	Timings timings{};

	ValueType menuKey = KeyValue('M');
	ValueType menuBlurEnabled = BoolValue(true);
	// TODO: add disabled settings, for people who already only support dx11, gray it out
	ValueType useDX11 = BoolValue(false);
	ValueType menuBlur = FloatValue(20.f);

	std::vector<ui::TextBox*> textBoxes = {};

	void threadsafeInit();
	void initSettings();

	bool shouldEject = false;
	bool hasInit = false;
};
