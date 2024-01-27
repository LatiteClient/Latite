#pragma once
#include "Hook.h"
#include "api/manager/StaticManager.h"

#include "impl/GeneralHooks.h"
#include "impl/LevelRendererHooks.h"
#include "impl/OptionHooks.h"
#include "impl/DXHooks.h"
#include "impl/AppPlatformHooks.h"
#include "impl/MinecraftGameHooks.h"
#include "impl/RenderControllerHooks.h"
#include "impl/ScreenViewHooks.h"
#include "impl/PacketHooks.h"
#include "impl/PlayerHooks.h"

class LatiteHooks final : public StaticManager<HookGroup,
	GenericHooks,
	LevelRendererHooks,
	OptionHooks,
	DXHooks,
	AppPlatformHooks,
	MinecraftGameHooks,
	RenderControllerHooks,
	ScreenViewHooks,
	PacketHooks,
	PlayerHooks> {
public:
	LatiteHooks() = default;
	~LatiteHooks() = default;

	void uninit();

	void enable();
	void disable();
private:
};