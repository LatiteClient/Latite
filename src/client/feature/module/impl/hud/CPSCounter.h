#pragma once
#include "../../TextModule.h"
#include "api/feature/setting/Setting.h"

class CPSCounter : public TextModule {
public:
	CPSCounter();

	std::wstringstream text(bool isDefault, bool inEditor) override;

	EnumData mode;
};