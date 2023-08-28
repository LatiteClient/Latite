#include "pch.h"

void util::doThrowIfFailed(HRESULT hr, int line, std::string func) {
	if (FAILED(hr)) {
		std::stringstream ss;
		ss << "DX assertion failed: " << func << ":" << line << " HRESULT: " << std::hex << hr;
		Logger::Fatal(ss.str());
#ifndef DEBUG
		// Set a breakpoint on this line to catch Win32 API errors.
		MessageBoxA(NULL, (std::string("An error has occured! Please screenshot and report this to the developer!\n") + ss.str()).c_str(), "DX assertion failed", MB_ICONERROR | MB_OK);
#endif
		//throw std::runtime_error("Error setting up DX.");
	}
}

d2d::Color::Color(StoredColor const& col) : ::Color(col.r, col.g, col.b, col.a) {
}

std::string d2d::Color::getHex() const {
	// thank you libfmt guy
	return std::format("{:02X}{:02X}{:02X}", static_cast<int>(std::round(r * 255.f)), static_cast<int>(std::round(g * 255.f)), static_cast<int>(std::round(b * 255.f)));
}
