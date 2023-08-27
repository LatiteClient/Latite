#pragma once
#include "util/LMath.h"

namespace SDK {
	class GameMode {
	public:
		class Player* plr;
	private:
		[[maybe_unused]] char pad[16];
	public:
		float lastBreakProgress;
		float breakProgress;

		virtual ~GameMode() = 0;
		virtual bool startDestroyBlock(BlockPos const&, uint8_t, bool&) = 0;
		virtual bool destroyBlock(BlockPos const&, uint8_t) = 0;
		virtual bool continueDestroyBlock(BlockPos const&, uint8_t, bool&) = 0;
		virtual void stopDestroyBlock(BlockPos const&) = 0;
		virtual void startBuildBlock(BlockPos const&, uint8_t) = 0;
		virtual bool buildBlock(BlockPos const&, uint8_t) = 0;
		virtual void continueBuildBlock(BlockPos const&, uint8_t) = 0;
		virtual void stopBuildBlock(void) = 0;
		virtual void tick(void) = 0;
		virtual void getPickRange(void*, bool) = 0;
		virtual void useItem(class ItemStack*) = 0;
		virtual void useItemOn(ItemStack*, BlockPos const&, unsigned char, Vec3 const&, class Block const*) = 0;
		virtual void interact(class Actor*, Vec3 const&) = 0;
		virtual void attack(Actor*) = 0;
		virtual void releaseUsingItem(void) = 0;
		virtual void setTrialMode(bool) = 0;
		virtual bool isInTrialMode(void) = 0;
		virtual void registerUpsellScreenCallback(void*) = 0;
	};
}