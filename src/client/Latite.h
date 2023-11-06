#pragma once
#include <string_view>
#include "api/eventing/Listenable.h"
#include "api/feature/setting/Setting.h"
#include <optional>
#include "misc/Timings.h"

namespace ui {
	class TextBox;
}

namespace sdk {
	class Font;
}

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
	[[nodiscard]] std::string getCommandPrefix() { return util::WStrToStr(std::get<TextValue>(commandPrefix).str); }

	void queueEject() noexcept;
	void initialize(HINSTANCE hInst);

	std::string getTextAsset(int resource);
	void downloadChakraCore();
	void initSettings();

	void queueForUIRender(std::function<void(SDK::MinecraftUIRenderContext* ctx)> callback);
	void queueForDXRender(std::function<void(ID2D1DeviceContext* ctx)> callback);

	Latite() = default;
	~Latite() = default;

	static constexpr std::string_view version = "v2.0.0b8";
	HINSTANCE dllInst = NULL;
	std::string gameVersion;

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

	std::vector<std::string> getLatiteUsers();

	[[nodiscard]] KeyValue getMenuKey() {
		return std::get<KeyValue>(menuKey);
	}

	[[nodiscard]] ColorValue getAccentColor() {
		return std::get<ColorValue>(accentColor);
	}

	[[nodiscard]] bool shouldForceDX11() {
		return std::get<BoolValue>(useDX11);
	}

	[[nodiscard]] bool shoulBlurHUD() {
		return std::get<BoolValue>(hudBlur);
	}

	[[nodiscard]] struct ID2D1BitmapBrush1* getHUDBlurBrush() {
		return hudBlurBrush.Get();
	}

	[[nodiscard]] bool useMinecraftRenderer() {
		return std::get<BoolValue>(minecraftRenderer);
	}

	[[nodiscard]] bool shouldRenderTextShadows() {
		return std::get<BoolValue>(textShadow);
	}

	[[nodiscard]] SDK::Font* getFont();

	void fetchLatiteUsers();

	void initAsset(int resource, std::wstring const& filename);

	int cInstOffs2 = 0;
	int cInstOffs = 0;
	int plrOffs2 = 0;
	int plrOffs = 0;

	void loadConfig(class SettingGroup& resolvedGroup);
private:
	bool downloadingAssets = false;
	std::vector<std::string> latiteUsers;
	std::vector<std::string> latiteUsersDirty;

	std::queue<std::function<void(SDK::MinecraftUIRenderContext* ctx)>> uiRenderQueue;
	std::queue<std::function<void(ID2D1DeviceContext* ctx)>> dxRenderQueue;

	Timings timings{};

	ValueType commandPrefix = TextValue(L".");
	ValueType menuKey = KeyValue('M');
	ValueType ejectKey = KeyValue(VK_END);
	ValueType hudBlur = BoolValue(false);
	ValueType hudBlurIntensity = FloatValue(10.f);
	ValueType menuBlurEnabled = BoolValue(true);
	// TODO: add disabled settings, for people who already only support dx11, gray it out
	ValueType useDX11 = BoolValue(false);
	ValueType menuBlur = FloatValue(20.f);
	ValueType accentColor = ColorValue(static_cast<float>(0x32) / 255.f, static_cast<float>(0x39) / 255.f, static_cast<float>(0x76) / 255.f);
	ValueType minimalViewBob = BoolValue(false);
	ValueType minecraftRenderer = BoolValue(false);
	ValueType textShadow = BoolValue(true);
	EnumData mcRendFont;

	std::vector<ui::TextBox*> textBoxes;
	ComPtr<struct ID2D1Bitmap1> hudBlurBitmap;
	ComPtr<struct ID2D1BitmapBrush1> hudBlurBrush;
	ComPtr<struct ID2D1Effect> gaussianBlurEffect;

	void threadsafeInit();
	void patchKey();

	void onUpdate(class Event& ev);
	void onKey(class Event& ev);
	void onClick(class Event& ev);
	void onChar(class Event& ev);
	void onRendererInit(class Event& ev);
	void onRendererCleanup(class Event& ev);
	void onFocusLost(class Event& ev);
	void onSuspended(class Event& ev);
	void onBobView(class Event& ev);
	void onLeaveGame(class Event& ev);
	void onRenderLayer(class Event& ev);
	void onRenderOverlay(class Event& ev);

	bool shouldEject = false;
	bool hasInit = false;
};

//extern "C" __declspec(dllexport) char* LatiteGetVersionsSupported();