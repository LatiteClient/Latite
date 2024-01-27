#pragma once
#include "../../HUDModule.h"
#include <sdk/common/client/gui/controls/UIControl.h>

class MovableBossbar : public HUDModule {
public:
	MovableBossbar();

	[[nodiscard]] virtual bool forceMinecraftRenderer() override { return true; }
	virtual void render(DrawUtil& ctx, bool isDefault, bool inEditor);

	void onRenderLayer(Event& ev);

	void updatePos();
private:
	SDK::UIControl* bossHealthGrid;

	Vec2 newPos = {};
};
