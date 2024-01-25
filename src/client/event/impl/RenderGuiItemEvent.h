#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include <sdk/common/network/packet/TextPacket.h>

class RenderGuiItemEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(RenderGuiItemEvent);

protected:
	SDK::ItemStack* item;
public:

	RenderGuiItemEvent(SDK::ItemStack* item)
		: item(item)
	{
	}
};