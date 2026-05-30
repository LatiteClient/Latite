#pragma once
#include <string>
#include <filesystem>
#include <vector>

namespace d2d {
	class Rect;
	struct Color;
}

struct Color;
struct HSV;

namespace util {
	extern std::filesystem::path GetRootPath();
	extern std::filesystem::path GetRoamingPath();
	extern std::filesystem::path GetLatitePath();
	extern std::wstring StrToWStr(std::string const& s);
	extern std::string WStrToStr(std::wstring const& ws);
	extern std::string RedactPrivatePaths(std::string text);
	extern std::wstring FormatWString(std::wstring const& formatString, std::vector<std::wstring> const& formatArgs);
	extern std::string Format(std::string const& s);
	extern std::wstring WFormat(std::wstring const& s);

	struct MinecraftFormatRun {
		std::wstring text;
		wchar_t colorCode = L'\0';
	};

	extern bool IsMinecraftFormattingCode(wchar_t code);
	extern wchar_t NormalizeMinecraftFormattingCode(wchar_t code);
	extern std::vector<MinecraftFormatRun> ParseMinecraftFormatting(std::wstring const& text);
	extern std::wstring StripMinecraftFormatting(std::wstring const& text);
	extern d2d::Color MinecraftFormattingColor(wchar_t code, d2d::Color const& fallback);
	extern std::wstring GetClipboardText();
	extern void SetClipboardText(std::wstring const& text);

	extern std::string KeyToString(int key);
	extern int StringToKey(std::string const& s);
	
	extern std::string ToLower(std::string const& s);
	extern std::string ToUpper(std::string const& s);
	extern std::vector<std::string> SplitString(std::string const& s, char delim);
	extern void PlaySoundUI(std::string const& sound, float volume = 1.f, float pitch = 1.f);

	extern Color LerpColorState(Color const& current, ::Color const& on, Color const& off, bool state, float speed = 3.f);

	extern struct HSV ColorToHSV(Color const& color);
	extern Color HSVToColor(HSV const& hsv);
	extern Vec4 RotToQuaternion(Vec2 const& rotatoin);
	extern Vec2 QuaternionToRot(const Vec4& quat);

	extern void KeepInBounds(d2d::Rect& targ, d2d::Rect const& bounds);
	extern std::string GetProcessorInfo();
}
