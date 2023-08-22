/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "Hooks.h"

#include "impl/GeneralHooks.h"
#include "impl/LevelRendererHooks.h"
#include "impl/OptionHooks.h"
#include "impl/DXHooks.h"
#include "impl/AppPlatformHooks.h"
#include "impl/MinecraftGameHooks.h"
#include "impl/RenderControllerHooks.h"
#include "impl/ScreenViewHooks.h"
#include "MinHook.h"

void LatiteHooks::init() {
	MH_Initialize();

	this->mutex.lock();
	this->items.push_back(std::make_shared<DXHooks>());
	this->items.push_back(std::make_shared<GenericHooks>());
	this->items.push_back(std::make_shared<LevelRendererHooks>());
	this->items.push_back(std::make_shared<OptionHooks>());
	this->items.push_back(std::make_shared<AppPlatformHooks>());
	this->items.push_back(std::make_shared<MinecraftGameHooks>());
	this->items.push_back(std::make_shared<RenderControllerHooks>());
	this->items.push_back(std::make_shared<ScreenViewHooks>());
	this->mutex.unlock();
}

void LatiteHooks::uninit() {
	MH_Uninitialize();
}

void LatiteHooks::enable() {
	MH_EnableHook(MH_ALL_HOOKS);
}

void LatiteHooks::disable() {
	MH_DisableHook(MH_ALL_HOOKS);
}
