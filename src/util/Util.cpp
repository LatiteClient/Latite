#include "pch.h"
#include "Util.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"
#include "sdk/common/client/renderer/game/LevelRendererPlayer.h"
#include "sdk/common/client/renderer/game/LevelRenderer.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"

#ifdef min
#undef min
#undef max
#endif

namespace util {
	namespace detail {
		template <typename T>
		class ListOr : public std::list<T> {
		public:
			ListOr(T const& item) : std::list<T>({item}) {}
			ListOr(std::initializer_list<T> list) : std::list<T>(list) {}
		};
	}

	std::unordered_map<size_t, const char*> KeyNames = {
		{0, "None"},
		{0x8, "Backspace"},
		{0x9, "Tab"},
		{0xC, "Clear"},
		{0xD, "Return"},
		{0x10, "Shift"},
		{0x11, "Ctrl"},
		{0x12, "Alt"},
		{0x13, "Pause"},
		{0x14, "CapsLock"},
		{0x15, "Kana"},
		{0x16, "IME On"},
		{0x17, "Junja"},
		{0x18, "Final"},
		{0x19, "Hanja"},
		{0x1A, "IME Off"},
		{0x1B, "Escape"},
		{0x20, "Space"},
		{0x21, "PageUp"},
		{0x22, "PageDown"},
		{0x23, "End"},
		{0x24, "Home"},
		{0x25, "Left"},
		{0x26, "Up"},
		{0x27, "Right"},
		{0x28, "Down"},
		{0x29, "Select"},
		{0x2A, "Print"},
		{0x2B, "Execute"},
		{0x2C, "PrintScreen"},
		{0x2D, "Insert"},
		{0x2E, "Delete"},
		{0x2F, "Help"},
		{0x30, "0"},
		{0x31, "1"},
		{0x32, "2"},
		{0x33, "3"},
		{0x34, "4"},
		{0x35, "5"},
		{0x36, "6"},
		{0x37, "7"},
		{0x38, "8"},
		{0x39, "9"},
		
		// A-Z calculated automatically

		{0x5B, "Windows"},
		{0x5C, "RightWindows"},
		{0x5D, "Applications"},
		{0x5F, "Sleep"},
		{0x60, "Numpad0"},
		{0x61, "Numpad1"},
		{0x62, "Numpad2"},
		{0x63, "Numpad3"},
		{0x64, "Numpad4"},
		{0x65, "Numpad5"},
		{0x66, "Numpad6"},
		{0x67, "Numpad7"},
		{0x68, "Numpad8"},
		{0x69, "Numpad9"},
		{0x6A, "Multiply"},
		{0x6B, "Add"},
		{0x6C, "Separator"},
		{0x6D, "Subtract"},
		{0x6E, "Decimal"},
		{0x6F, "Divide"},

		{0x70, "F1"},
		{0x71, "F2"},
		{0x72, "F3"},
		{0x73, "F4"},
		{0x74, "F5"},
		{0x75, "F6"},
		{0x76, "F7"},
		{0x77, "F8"},
		{0x78, "F9"},
		{0x79, "F10"},
		{0x7A, "F11"},
		{0x7B, "F12"},
		{0x7C, "F13"},
		{0x7D, "F14"},
		{0x7E, "F15"},
		{0x7F, "F16"},
		{0x80, "F17"},
		{0x81, "F18"},
		{0x82, "F19"},
		{0x83, "F20"},
		{0x84, "F21"},
		{0x85, "F22"},
		{0x86, "F23"},
		{0x87, "F24"},

		{0x90, "NumLock"},
		{0x91, "ScrollLock"},
		{0xA2, "LCtrl"},
		{0xA3, "RCtrl"},
		{0xAD, "VolumeMute"},
		{0xAE, "VolumeDown"},
		{0xAF, "VolumeUp"},
		{0xB0, "NextTrack"},
		{0xB1, "PrevTrack"},
		{0xB2, "MediaStop"},
		{0xB3, "PlayPause"},
		{0xB4, "LaunchMail"},
	};
}

std::filesystem::path util::GetRootPath() {
	wchar_t* env;
	size_t size;
	if (!_wdupenv_s(&env, &size, L"localappdata") && env) {
		auto str = std::wstring(env).substr(0, lstrlenW(env) - 2);
		delete env;
		return str;
	}
	return std::wstring();
}

std::filesystem::path util::GetRoamingPath() {
	wchar_t* env;
	size_t size;
	if (!_wdupenv_s(&env, &size, L"localappdata") && env) {
		auto str = std::wstring(env).substr(0, lstrlenW(env) - 2) + L"RoamingState";
		delete env;
		return str;
	}
	return std::wstring();
}

std::filesystem::path util::GetLatitePath() {
	// TODO: Rename to Latite
	return GetRoamingPath()/"LatiteRecode";
}

std::wstring util::StrToWStr(std::string const& s) {
	int slength = static_cast<int>(s.length()) + 1;
	int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string util::WStrToStr(std::wstring const& ws) {
	std::string ret;
	int len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), static_cast<int>(ws.size()), NULL, 0, NULL, NULL);
	if (len > 0) {
		ret.resize(len);
		WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), static_cast<int>(ws.size()), &ret[0], len, NULL, NULL);
	}
	return ret;
}

std::string util::Format(std::string const& s) {
	std::string out;

	for (auto& ch : s) {
		if (ch == '&') {
			out += (char)0xC2;
			out += (char)0xA7;
		}
		else out += ch;
	}
	return out;
}

std::wstring util::GetClipboardText() {
	// Try opening the clipboard
	if (!OpenClipboard(nullptr)) {
		return L"";
	}

	// Get handle of clipboard object for ANSI text
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == nullptr)
		return L"";

	// Lock the handle to get the actual text pointer
	wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
	if (pszText == nullptr)
		return L"";

	// Save text in a string class instance
	std::wstring text(pszText);

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	return text;
}

void util::SetClipboardText(std::wstring const& text) {
	if (OpenClipboard(NULL)) {
		EmptyClipboard();
		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, (text.size() * 2) + 1); // \0
		if (!hg) {
			CloseClipboard();
		}
		else {
			memcpy(GlobalLock(hg), text.c_str(), (text.size() * 2) + 1);
			GlobalUnlock(hg);
			SetClipboardData(CF_UNICODETEXT, hg);
			CloseClipboard();
			GlobalFree(hg);
		}
	}
}

std::string util::KeyToString(int key) {
	if (key > 0x40 && key < 0x5B) {
		return std::string(1, (char)key);
	}

	auto it = KeyNames.find(key);
	if (it == KeyNames.end()) {
		return "Unknown";
	}

	return it->second;
}

int util::StringToKey(std::string const& s) {
	if (s.empty()) return 0;
	if (s[0] > 0x40 && s[0] < 0x5B) {
		return (int)s[0];
	}

	for (auto& ent : KeyNames) {
		if (ToLower(ent.second) == ToLower(s)) return static_cast<int>(ent.first);
	}
	return 0;
}

std::string util::ToLower(std::string const& s) {
	std::string ret = s;
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	return ret;
}

std::string util::ToUpper(std::string const& s) {
	std::string ret = s;
	std::transform(ret.begin(), ret.end(), ret.begin(), [](char c) {
		return c - (char)20;
		});
	return ret;
}

std::vector<std::string> util::SplitString(std::string const& s, char delim) {
	std::vector<std::string> ret = {};
	std::string word = "";

	for (size_t i = 0; i < s.size(); i++) {
		if (s[i] == delim) {
			ret.push_back(word);
			word = "";
			continue;
		}
		word += s[i];
	}

	if (word.size() > 0) {
		ret.push_back(word);
	}

	return ret;
}

void util::PlaySoundUI(std::string const& sound, float volume, float pitch) {
	auto cInst = SDK::ClientInstance::get();
	auto lr = cInst->levelRenderer;
	if (lr) {
		cInst->minecraft->getLevel()->playSoundEvent(sound, lr->getLevelRendererPlayer()->getOrigin(), volume, pitch);
	} // TODO: make it work outside world
}

Color util::LerpColorState(Color const& current, Color const& on, Color const& off, bool state, float speed) {
	Color ret = current;
	float t = Latite::getRenderer().getDeltaTime()* (speed / 10.f);
	ret.r = std::lerp(current.r, state ? on.r : off.r, t);
	ret.g = std::lerp(current.g, state ? on.g : off.g, t);
	ret.b = std::lerp(current.b, state ? on.b : off.b, t);
	ret.a = std::lerp(current.a, state ? on.a : off.a, t);
	return ret;
}


HSV util::ColorToHSV(Color const& color) {
	HSV hsv{ 0.f, 0.f, 0.f };

	float minVal = std::min(color.r, std::min(color.g, color.b));
	float maxVal = std::max(color.r, std::max(color.g, color.b));
	float delta = maxVal - minVal;

	hsv.v = maxVal;

	if (delta == 0) {
		hsv.h = 0;
		hsv.s = 0;
	}
	else {
		hsv.s = delta / maxVal;

		if (color.r == maxVal) {
			hsv.h = (color.g - color.b) / delta;
		}
		else if (color.g == maxVal) {
			hsv.h = 2 + (color.b - color.r) / delta;
		}
		else {
			hsv.h = 4 + (color.r - color.g) / delta;
		}

		hsv.h *= 60;

		if (hsv.h < 0) {
			hsv.h += 360;
		}
	}

	return hsv;
}

Color util::HSVToColor(HSV const& hsv) {
	Color color;
	color.a = 1.f;
	float hue = hsv.h;
	while (hue >= 360.f) hue -= 360.f;

	if (hsv.s == 0) {
		// grayscale
		color.r = hsv.v;
		color.g = hsv.v;
		color.b = hsv.v;
	}
	else {
		int i = static_cast<int>(std::floor(hue / 60.f));
		float f = hue / 60.f - i;
		float p = hsv.v * (1.f - hsv.s);
		float q = hsv.v * (1.f - hsv.s * f);
		float t = hsv.v * (1.f - hsv.s * (1.f - f));

		switch (i) {
		case 0:
			color.r = hsv.v;
			color.g = t;
			color.b = p;
			break;
		case 1:
			color.r = q;
			color.g = hsv.v;
			color.b = p;
			break;
		case 2:
			color.r = p;
			color.g = hsv.v;
			color.b = t;
			break;
		case 3:
			color.r = p;
			color.g = q;
			color.b = hsv.v;
			break;
		case 4:
			color.r = t;
			color.g = p;
			color.b = hsv.v;
			break;
		default:
			color.r = hsv.v;
			color.g = p;
			color.b = q;
			break;
		}
	}

	return color;
}

Vec4 util::RotToQuaternion(Vec2 const& rot) {
	constexpr float invRadDegrees = pi_f / 180.f;

	float pitch = rot.x * invRadDegrees;
	float yaw = rot.y * invRadDegrees;
	float roll = 0.f;

	float cx = std::cos(pitch * 0.5f);
	float sx = std::sin(pitch * 0.5f);
	float cy = std::cos(yaw * 0.5f);
	float sy = std::sin(yaw * 0.5f);
	float cz = std::cos(roll * 0.5f);
	float sz = std::sin(roll * 0.5f);

	return Vec4{
		(cx * cy * sz) - (sx * sy * cz), // quat.x
		(cx * cy * cz) + (sx * sy * sz), // quat.y
		(sx * cy * cz) - (cx * sy * sz), // quat.z
		(cx * sy * cz) + (sx * cy * sz)  // quat.w
	};
}

Vec2 util::QuaternionToRot(const Vec4& quat) {
	constexpr float radDegrees = 180.f / pi_f;

	float pitch = 0.f;
	float yaw = 0.f;
	[[maybe_unused]] float roll = 0.f;

	// roll (x-axis rotation)
	float sinr_cosp = 2.f * (quat.x * quat.y + quat.z * quat.w);
	float cosr_cosp = 1.f - (2.f * (quat.y * quat.y + quat.z * quat.z));
	roll = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = 2.f * (quat.y * quat.w - quat.x * quat.z);
	if (std::abs(sinp) >= 1.f) {
		pitch = std::copysign(pi_f / 2.f, sinp);  // use 90 degrees if out of range
	}
	else {
		pitch = std::asin(sinp);
	}

	// yaw (z-axis rotation)
	float siny_cosp = 2.f * (quat.x * quat.w + quat.y * quat.z);
	float cosy_cosp = 1.f - (2.f * (quat.z * quat.z + quat.w * quat.w));
	yaw = std::atan2(siny_cosp, cosy_cosp);

	return Vec2{
		pitch * radDegrees,
		yaw * radDegrees
	};
}

void util::KeepInBounds(d2d::Rect& targ, d2d::Rect const& bounds) {
	if (targ.left < bounds.left) {
		Vec2 modPos = targ.getPos();
		targ.setPos({ bounds.left, modPos.y });
	}

	if (targ.top < bounds.top) {
		Vec2 modPos = targ.getPos();
		targ.setPos({ modPos.x, bounds.top });
	}

	if (targ.right > bounds.right) {
		Vec2 modPos = targ.getPos();
		targ.setPos({ bounds.right - targ.getWidth(), modPos.y });
	}

	if (targ.bottom > bounds.bottom) {
		Vec2 modPos = targ.getPos();
		targ.setPos({ modPos.x, bounds.bottom - targ.getHeight() });
	}
}

std::string util::GetProcessorInfo() {
	constexpr std::array<int, 3> cpuIds = { 0x80000002, 0x80000003, 0x80000004 };
	std::array<int, 4> data{};
	std::string model;

	for (auto& id : cpuIds) {
		__cpuid(data.data(), id);
		model += std::string(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(int));
	}

	return model;
}
