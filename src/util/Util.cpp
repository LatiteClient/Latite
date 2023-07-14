#include "Util.h"
#include "pch.h"

std::filesystem::path util::getRoamingPath()
{
	char* env;
	size_t size;
	if (!_dupenv_s(&env, &size, "localappdata")) {
		auto str = std::string(env).substr(0, strlen(env) - 2) + "RoamingState";
		delete env;
		return str;
	}
	return std::wstring();
}

std::filesystem::path util::getLatitePath()
{
	// TODO: Rename to Latite
	return getRoamingPath()/"LatiteRecode";
}

std::wstring util::strToWstr(std::string const& s)
{
	int slength = static_cast<int>(s.length()) + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string util::wstrToStr(std::wstring const& ws)
{
	int len = WideCharToMultiByte(CP_ACP, 0, ws.c_str(), static_cast<int>(ws.size() + 1), 0, 0, 0, 0);
	char* buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, ws.c_str(), static_cast<int>(ws.size() + 1), buf, len, 0, 0);
	std::string r(buf);
	delete[] buf;
	return r;
}

uintptr_t util::ScanSignature(const char* signature) {
	return memory::ScanSignature(signature, "Minecraft.Windows.exe");
}
