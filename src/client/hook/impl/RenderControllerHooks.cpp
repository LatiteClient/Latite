#include "RenderControllerHooks.h"
#include "sdk/signature/storage.h"
#include "client/event/Eventing.h"
#include "client/event/impl/OverlayColorEvent.h"
#include "util/Util.h"
#include "sdk/common/world/actor/Actor.h"
#include "sdk/Util.h"

namespace {
	std::shared_ptr<Hook> GetOverlayColorHook;
}

Color* RenderControllerHooks::getOverlayColor(void* thisptr, Color* out, void* ent) {
	GetOverlayColorHook->oFunc<decltype(&getOverlayColor)>()(thisptr, out, ent);
	OverlayColorEvent ev{ *out, util::directAccess<sdk::Actor*>(ent, 0x38) }; // xref: getOverlayColor itself
	{
		Eventing::get().dispatchEvent(ev);
		*out = ev.getColor();
	}
	return out;
}

RenderControllerHooks::RenderControllerHooks() {
	GetOverlayColorHook = addHook(Signatures::RenderController_getOverlayColor.result, getOverlayColor, "RenderController::getOverlayColor");
}
