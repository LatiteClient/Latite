#include "pch.h"
#include "RenderControllerHooks.h"
#include "client/event/Eventing.h"
#include "client/event/events/OverlayColorEvent.h"
#include "mc/common/world/actor/Actor.h"
#include "mc/Util.h"

namespace {
	std::shared_ptr<Hook> GetOverlayColorHook;
}

Color* RenderControllerHooks::getOverlayColor(void* thisptr, Color* out, void* ent) {
	GetOverlayColorHook->oFunc<decltype(&getOverlayColor)>()(thisptr, out, ent);
	OverlayColorEvent ev{ *out, hat::member_at<SDK::Actor*>(ent, 0x38) }; // xref: getOverlayColor itself
	{
		Eventing::get().dispatch(ev);
		*out = ev.getColor();
	}
	return out;
}

RenderControllerHooks::RenderControllerHooks() {
	GetOverlayColorHook = addHook(Signatures::RenderController_getOverlayColor.result, getOverlayColor, "RenderController::getOverlayColor");
}
