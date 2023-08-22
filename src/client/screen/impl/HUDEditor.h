/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../Screen.h"
#include "util/LMath.h"
#include "util/DxUtil.h"
#include "client/feature/module/HudModule.h"

class HUDEditor : public Screen {
public:
	HUDEditor();

	std::string getName() override { return "HUDEditor"; }
	void onEnable(bool ignoreAnims) override;
	void onDisable() override;
private:
	void onRender(Event& ev);
	void onClick(Event& ev);
	void onRenderLayer(Event& ev);
	void renderModule(class HUDModule* mod);
	void doDragging();
	void doSnapping(Vec2 const&);
	void keepModulesInBounds();

	class HUDModule* dragMod;

	float anim = 0.f;
	Vec2 dragOffset = {};
	std::vector<d2d::Rect> controls = {};

	struct SnapLine {
		float left, middle, right;
		d2d::Color color = d2d::Colors::WHITE;
		float thickness = 1.f;

		SnapLine(HUDModule* dragMod, float num, bool horizontal) {
			if (horizontal) {
				float height = dragMod->getRect().getHeight();
				right = num;
				left = num - height;
				middle = num - (height / 2.f);
				return;
			}
			float width = dragMod->getRect().getWidth();
			right = num;
			left = num - width;
			middle = num - (width / 2.f);
		}
	};
};