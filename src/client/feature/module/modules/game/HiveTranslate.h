#pragma once
#include <client/feature/module/Module.h>

class HiveTranslate : public Module {
public:
	HiveTranslate();
	virtual ~HiveTranslate() {};

	void onText(Event&);

private:
	ValueType targetLanguage = TextValue(L"en");
};