#pragma once
#include <client/feature/module/Module.h>

class AutoQueue : public Module {
public:
	AutoQueue();
	virtual ~AutoQueue() {};

	void onPacketRecieve(Event& evG);
	void onText(Event& evG);

private:
	void queueHiveGame();
	std::wstring extractHiveGamemode(std::wstring& message);
	std::wstring currentHiveGamemode = L"";

	ValueType filterSensitiveInfo = BoolValue(false);
	ValueType soloMode = BoolValue(false);
	bool sentCmd = false;
	bool supress = false;
};