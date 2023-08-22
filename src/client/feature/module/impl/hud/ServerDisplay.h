#pragma once
#include "../../TextModule.h"

class ServerDisplay : public TextModule {
private:
	static constexpr int port_mixed = 0;
	static constexpr int port_constant = 1;
	static constexpr int port_none = 2;
	EnumData port;

	ValueType showServerName = BoolValue(true);
public:
	ServerDisplay();
	~ServerDisplay() = default;
protected:

	std::wstringstream text(bool isDefault, bool inEditor) override;

};