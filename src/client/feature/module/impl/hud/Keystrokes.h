#pragma once
#include "../../HUDModule.h"

class Keystrokes : public HUDModule {
public:
	Keystrokes();

	void render(DXContext& dc, bool, bool) override;
private:

};