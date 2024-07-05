#pragma once
#include "../../HUDModule.h"
#include <sdk/common/client/gui/controls/UIControl.h>

class MovableCoordinates : public HUDModule {
public:
	MovableCoordinates();

	[[nodiscard]] virtual bool forceMinecraftRenderer() override { return true; }
	virtual void render(DrawUtil& ctx, bool isDefault, bool inEditor);

	void onRenderLayer(Event& ev);
	void updatePos();
private:
	SDK::UIControl* vanillaCoordinates = nullptr;
	ValueType hideVanillaCoordinates = BoolValue(false);
};
