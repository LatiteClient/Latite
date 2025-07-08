#pragma once
#include <string>
#include <vector>
#include <functional>
#include "../../ItemStack.h"

namespace SDK {
	class Inventory { /*copied from bedrock server, not perfect*/
	public:
		// EatVtable.js (in.txt -> out.txt)
		virtual ~Inventory() = 0;
		virtual void init(void) = 0;
		virtual void unk(void) = 0;
		virtual void addContentChangeListener(class ContainerContentChangeListener*) = 0;
		virtual void removeContentChangeListener(ContainerContentChangeListener*) = 0;
		virtual void addRemovedListener(class ContainerRemovedListener*) = 0;
		virtual void removeRemovedListener(class ContainerRemovedListener*) = 0;
		virtual ItemStack* getItem(int) = 0;

		// the other stuff aren't updated to 1.21 lol

	private:
		virtual bool hasRoomForItem(ItemStack const&) = 0;
		virtual void addItem(ItemStack&) = 0;
		virtual char addItemToFirstEmptySlot(ItemStack&) = 0;
		virtual void setItem(int, ItemStack const&) = 0;
		virtual void setItemWithForceBalance(int, ItemStack const&, bool) = 0;
		virtual void removeItem(int, int) = 0;
		virtual void removeAllItems(void) = 0;
		virtual void dropContents(class BlockSource&, struct Vec3 const&, bool) = 0;
		virtual void getContainerSize(void) = 0;
		virtual long long getMaxStackSize(void) = 0;
		virtual void nullsub_7(void) = 0;
		virtual void nullsub_8(void) = 0;
		virtual void getSlotCopies(void) = 0;
		virtual std::vector<const ItemStack*, std::allocator<const ItemStack*>> getSlots(void) = 0;
		virtual void getItemCount(ItemStack const&) = 0;
		virtual void findFirstSlotForItem(ItemStack const&) = 0;
		// return true functions
		virtual bool nullsub_9(void) = 0;
		virtual bool nullsub_10(void) = 0;
		virtual void setContainerChanged(int) = 0;
		virtual void setContainerMoved(void) = 0;
		virtual void** setCustomName(std::string const&) = 0;
		virtual bool hasCustomName(void) = 0;
		virtual void readAdditionalSaveData(class CompoundTag const&) = 0;
		virtual void addAdditionalSaveData(CompoundTag&) = 0;
		virtual void createTransactionContext(std::function<void(Inventory&, int, ItemStack const&, ItemStack const&)>, std::function<void(void)>) = 0;
		virtual void nullsub_11(void) = 0;
		virtual bool isEmpty(void) = 0;
	};
}