#pragma once
#include "../../HUDModule.h"
#include <sdk/common/client/gui/controls/UIControl.h>

class MovableScoreboard : public HUDModule {
public:
	MovableScoreboard();

	[[nodiscard]] virtual bool forceMinecraftRenderer() override { return true; }
	virtual void render(DrawUtil& ctx, bool isDefault, bool inEditor);

	void onRenderLayer(Event& ev);

	void updatePos();
private:
	SDK::UIControl* sidebar;

	Vec2 newPos = {};
};
