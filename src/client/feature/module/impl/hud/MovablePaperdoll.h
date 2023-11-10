#pragma once
#include "../../HUDModule.h"
#include <sdk/common/client/gui/controls/UIControl.h>

class MovablePaperdoll : public HUDModule {
public:
	MovablePaperdoll();

	[[nodiscard]] virtual bool forceMinecraftRenderer() override { return true; }
	virtual void render(DrawUtil& ctx, bool isDefault, bool inEditor);

	void onRenderLayer(Event& ev);

private:
	ValueType alwaysShow = BoolValue(false);
	SDK::UIControl* hudPlayer;
};
