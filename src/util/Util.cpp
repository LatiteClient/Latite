#include "Util.h"

std::wstring util::getRoamingPath()
{
	if (!_dupenv_s(&env, &size, "localappdata")) {
		auto str = std::string(env).substr(0, strlen(env) - 2) + "RoamingState";
		delete env;
		return str;
	}
	return std::wstring();
}

std::wstring util::getLatitePath()
{
	return std::wstring();
}
