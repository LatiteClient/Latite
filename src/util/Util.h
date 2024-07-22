#pragma once
#include <string>
#include <filesystem>
#include "api/scanner/Scanner.h"

namespace d2d {
	class Rect;
}

struct Color;
struct HSV;

namespace util {
	extern std::filesystem::path GetRootPath();
	extern std::filesystem::path GetRoamingPath();
	extern std::filesystem::path GetLatitePath();
	extern std::wstring StrToWStr(std::string const& s);
	extern std::string WStrToStr(std::wstring const& ws);
	extern std::wstring FormatWString(std::wstring formatString, std::vector<std::wstring> formatArgs);
	extern std::string Format(std::string const& s);
	extern std::wstring WFormat(std::wstring const& s);
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

	extern inline uintptr_t FindSignature(std::string_view signature) {
		return memory::findSignature(signature, "Minecraft.Windows.exe");
	}

	extern void KeepInBounds(d2d::Rect& targ, d2d::Rect const& bounds);
	extern std::string GetProcessorInfo();
}
