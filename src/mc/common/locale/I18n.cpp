#include "pch.h"
#include "I18n.h"

SDK::I18n* SDK::I18n::get() {
	if (!Signatures::I18n_getI18n.result) return nullptr;

	return reinterpret_cast<I18n*(__fastcall*)()>(Signatures::I18n_getI18n.result)();
}

std::string SDK::I18n::get(std::string const& key) {
	auto vtable = *reinterpret_cast<void***>(this);
	if (!vtable || !vtable[0x10]) return {};

	std::string result;
	// Current Windows I18n callers use slot 0x10 and pass an empty locale to
	// select Minecraft's active language.
	std::shared_ptr<::Localization const> locale;
	using GetFunction = std::string*(__fastcall*)(
		I18n*, std::string*, std::string const*, std::shared_ptr<::Localization const>*);
	reinterpret_cast<GetFunction>(vtable[0x10])(this, &result, &key, &locale);
	return result;
}
