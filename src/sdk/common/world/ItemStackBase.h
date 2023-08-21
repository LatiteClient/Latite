#pragma once
#include <chrono>
#include "Item.h"

namespace SDK {
	class ItemStackBase {
	public:
		// ItemStackBase
		Item** item; //0x0008 WeakPtr<Item>
		std::unique_ptr<class CompoundTag> tag;
		class Block* block; // 0x0018
		short aux; // 0x0020
		uint8_t itemCount;
		bool valid;
		std::chrono::steady_clock::time_point pickupTime;
		bool showPickup;
		std::vector<class BlockLegacy*> canPlaceOn;
		uintptr_t canPlaceOnHash;
		std::vector<class BlockLegacy*> canDestroy;
		uintptr_t blockingTick[3]; // Tick

		std::string getHoverName();
		short getDamageValue();

		virtual ~ItemStackBase() = 0;
	};

	static_assert(sizeof(ItemStackBase) == 0x88);
}