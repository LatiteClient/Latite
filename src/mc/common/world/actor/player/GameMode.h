#pragma once
#include "InteractionResult.h"
#include "util/LMath.h"

#include <cstdint>
#include <functional>

namespace SDK {
    enum class InputMode : uint32_t;

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
        virtual bool continueDestroyBlock(BlockPos const&, uint8_t, Vec3 const&, bool&) = 0;
        virtual void stopDestroyBlock(BlockPos const&) = 0;
        virtual void startBuildBlock(BlockPos const&, uint8_t, uint8_t = 0) = 0;
        virtual bool buildBlock(BlockPos const&, uint8_t, uint8_t, bool) = 0;
        virtual void continueBuildBlock(BlockPos const&, uint8_t, uint8_t = 0) = 0;
        virtual void stopBuildBlock() = 0;

        bool buildBlock(BlockPos const& pos, uint8_t face, bool isFirstEvent) {
            return buildBlock(pos, face, 0, isFirstEvent);
        }
        virtual void tick() = 0;
        virtual float getPickRange(InputMode const&) = 0;
        virtual bool useItem(class ItemStack&) = 0;
        virtual bool useItemAsAttack(ItemStack&, Vec3 const&) = 0;
        virtual InteractionResult useItemOn(ItemStack&, BlockPos const&, uint8_t, Vec3 const&, class Block const*,
                                            bool) = 0;
        virtual bool interact(class Actor&, Vec3 const&) = 0;
        virtual bool attack(Actor&, Vec3 const&) = 0;
        virtual void releaseUsingItem() = 0;
        virtual void setTrialMode(bool) = 0;
        virtual bool isInTrialMode() = 0;
        virtual void registerUpsellScreenCallback(std::function<void(bool)>) = 0;
    };
}
