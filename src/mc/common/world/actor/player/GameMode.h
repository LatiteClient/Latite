#pragma once
#include "util/LMath.h"

namespace SDK {

class InteractionResult {
public:
    bool mSuccess : 1;
    bool mSwing : 1;
};

class GameMode {
public:
    class Player* plr;

private:
    [[maybe_unused]] char pad[16];

public:
    float lastBreakProgress;
    float breakProgress;

    virtual ~GameMode() = 0;

    virtual bool startDestroyBlock(
        BlockPos const&,
        uint8_t,
        bool&
    ) = 0;

    virtual bool destroyBlock(
        BlockPos const&,
        uint8_t
    ) = 0;

    virtual bool continueDestroyBlock(
        BlockPos const&,
        uint8_t,
        Vec3 const&,
        bool&
    ) = 0;

    virtual void stopDestroyBlock(
        BlockPos const&
    ) = 0;

    virtual void startBuildBlock(
        BlockPos const&,
        uint8_t
    ) = 0;

    virtual bool buildBlock(
        BlockPos const&,
        uint8_t,
        bool
    ) = 0;

    virtual void continueBuildBlock(
        BlockPos const&,
        uint8_t
    ) = 0;

    virtual void stopBuildBlock() = 0;

    virtual void tick() = 0;

    virtual float getPickRange(
        void*
    ) = 0;

    virtual bool useItem(
        ItemStack*
    ) = 0;

    virtual bool useItemAsAttack(
        ItemStack*,
        Vec3 const&
    ) = 0;

    virtual InteractionResult useItemOn(
        ItemStack*,
        BlockPos const&,
        uint8_t,
        Vec3 const&,
        Block const*,
        bool
    ) = 0;

    virtual bool interact(
        Actor*,
        Vec3 const&
    ) = 0;

    virtual bool attack(
        Actor*,
        Vec3 const&
    ) = 0;

    virtual void releaseUsingItem() = 0;

    virtual void setTrialMode(bool) = 0;

    virtual bool isInTrialMode() = 0;

    virtual void registerUpsellScreenCallback(
        void*
    ) = 0;
};

}
