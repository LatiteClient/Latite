#include "ClickGUI.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RendererCleanupEvent.h"
#include "client/event/impl/RendererInitEvent.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/ClickEvent.h"
#include "client/event/impl/CharEvent.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"
#include "client/feature/module/Module.h"
#include "client/feature/module/ModuleManager.h"
#include "util/Util.h"
#include "util/DxContext.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"
#include "client/render/Assets.h"
#include "client/config/ConfigManager.h"

#include "../ScreenManager.h"

#ifdef min
#undef min
#undef max
#endif
#include <client/feature/module/HUDModule.h>

#include <optional>
#include <array>

using FontSelection = Renderer::FontSelection;
using RectF = d2d::Rect;

float calcAnim = 0.f;

namespace {
	static constexpr float setting_height_relative = 0.0168f; // 0.0168
}

ClickGUI::ClickGUI() : Screen("ClickGUI") {
	Latite::get().addTextBox(&this->searchTextBox);

	Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&ClickGUI::onRender, 1, true);
	Eventing::get().listen<RendererCleanupEvent>(this, (EventListenerFunc)&ClickGUI::onCleanup, 1, true);
	Eventing::get().listen<RendererInitEvent>(this, (EventListenerFunc)&ClickGUI::onInit, 1, true);
	Eventing::get().listen<KeyUpdateEvent>(this, (EventListenerFunc)&ClickGUI::onKey, 1);
	Eventing::get().listen<ClickEvent>(this, (EventListenerFunc)&ClickGUI::onClick, 1);
}

void ClickGUI::onRender(Event&) {
	if (!isActive() && calcAnim < 0.03f) {
		calcAnim = 0.f;
		return;
	}
	bool shouldArrow = true;

	if (colorPicker.setting) {
		addLayer(cPickerRect);
	}

	DXContext dc;

	if (!isActive()) justClicked = { false, false, false };
	if (isActive()) sdk::ClientInstance::get()->releaseCursor();
	dc.ctx->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	Vec2& cursorPos = sdk::ClientInstance::get()->cursorPos;

	//auto& ev = reinterpret_cast<RenderOverlayEvent&>(evGeneric);
	auto& rend = Latite::getRenderer();
	auto ss = rend.getScreenSize();

	adaptedScale = ss.width / 1920.f;

	float guiX = ss.width / 4.f;
	float guiY = ss.height / 4.f;
	{
		float totalWidth = ss.height * (16.f / 9.f);;

		float realGuiX = totalWidth / 2.f;

		guiX = (ss.width / 2.25f) - (realGuiX / 2.f);
		guiY = (ss.height / 5.f);
	}

	rect = { guiX, guiY, ss.width - guiX, ss.height - guiY };
	float guiWidth = rect.getWidth();

	if (Latite::get().getMenuBlur()) dc.drawGaussianBlur(Latite::get().getMenuBlur().value() * (isActive() ? 1.f : calcAnim));

	// Animation
	D2D1::Matrix3x2F oTransform;
	D2D1::Matrix3x2F currentMatr;
	if (isActive()) {
		dc.ctx->GetTransform(&oTransform);

		dc.ctx->SetTransform(D2D1::Matrix3x2F::Scale({ calcAnim, calcAnim }, D2D1_POINT_2F(rect.center().x, rect.center().y)));
		dc.ctx->GetTransform(&currentMatr);
	}
	calcAnim = std::lerp(calcAnim, isActive() ? 1.f : 0.f, sdk::ClientInstance::get()->minecraft->timer->alpha * 0.2f);

	d2d::Color outline = d2d::Color::RGB(0, 0, 0);
	outline.a = 0.28f;
	d2d::Color rcColor = d2d::Color::RGB(0x7, 0x7, 0x7);
	rcColor.a = 0.75f;
	rect.round();

	if (!isActive()) return;
	// Shadow effect stuff
	auto shadowEffect = Latite::getRenderer().getShadowEffect();
	shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::Vector4F(0.f, 0.f, 0.f, 0.1f));
	auto affineTransformEffect = Latite::getRenderer().getAffineTransformEffect();

	D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Translation(10.f * adaptedScale, 5.f * adaptedScale);
	affineTransformEffect->SetInputEffect(0, shadowEffect);
	affineTransformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, mat);
	// Shadow effect bitmap
	auto myBitmap = rend.getBitmap();
	//

	// Menu Rect
	dc.fillRoundedRectangle(rect, rcColor, 19.f * adaptedScale);
	dc.drawRoundedRectangle(rect, outline, 19.f * adaptedScale, 4.f * adaptedScale, DXContext::OutlinePosition::Outside);
	//dc.drawText({ 0, 0, 200, 30 }, "Hello world!", D2D1::ColorF::White, 30.f, DWRITE_TEXT_ALIGNMENT_CENTER);

	float offX = 0.01689f * rect.getWidth();
	float offY = 0.03191f * rect.getHeight();
	float imgSize = 0.05338f * rect.getWidth();

	D2D1_RECT_F logoRect = { rect.left + offX, rect.top + offY, rect.left + offX + imgSize, rect.top + offY + imgSize };

	// Latite Logo + text
	{
		{
			auto bmp = Latite::getAssets().latiteLogo.getBitmap();

			D2D1::Matrix3x2F oMat;
			auto sz = Latite::getRenderer().getScreenSize();

			D2D1::Matrix3x2F m;

			//dc.ctx->GetTransform(&m);
			//dc.ctx->SetTransform(D2D1::Matrix3x2F::Scale(41.f / sz.width, 41.f / sz.height) * D2D1::Matrix3x2F::Translation(logoRect.left, logoRect.top) * m);
			dc.ctx->DrawBitmap(bmp, logoRect, 1.f);
			//dc.ctx->DrawImage(compositeEffect.Get(), { 0.f, 0.f });
			//dc.ctx->SetTransform(m);
		}


		// Latite Text
		dc.drawText({ logoRect.right + 9.f * adaptedScale, logoRect.top + 3.f * adaptedScale, logoRect.right + 500.f, logoRect.top + 50.f * adaptedScale }, L"Latite Client", d2d::Color(1.f, 1.f, 1.f, 1.f), FontSelection::Light, 25.f * adaptedScale, DWRITE_TEXT_ALIGNMENT_LEADING);
	}

	// X button / other menus
	{
		// x btn calc
		float xOffs = rect.getWidth() * 0.02217f;
		float yOffs = rect.getHeight() * 0.04432f;

		float xWidth = rect.getWidth() * 0.02323f;
		float xHeight = xWidth;//rect.getHeight() * 0.04078f;

		RectF xRect = { rect.right - xOffs - xWidth, rect.top + yOffs, rect.right - xOffs, rect.top + yOffs + xHeight };

		auto bmp = Latite::getAssets().xIcon.getBitmap();
		dc.ctx->DrawBitmap(bmp, xRect, 1.f);

		if (shouldSelect(xRect, cursorPos)) {
			if (justClicked[0]) {
				playClickSound();
				close();
			}
		}

		float betw = rect.getWidth() * 0.01795f;
		if (tab == SETTINGS) {
			float right = xRect.left - betw;
			RectF backArrowRect = { right - xWidth, xRect.top, right, xRect.bottom };
			{
				dc.ctx->DrawBitmap(Latite::getAssets().arrowBackIcon.getBitmap(), backArrowRect);
			}
			if (shouldSelect(backArrowRect, cursorPos)) {
				if (justClicked[0]) {
					playClickSound();
					this->tab = MODULES;
				}
			}
		}
		else if (tab == MODULES) {
			RectF hudEditRect;
			{
				float hudEditWidth = rect.getWidth() * 0.02851f;
				float hudEditHeight = rect.getHeight() * 0.04432f;
				float hudEditRight = xRect.left - betw;

				hudEditRect = { hudEditRight - hudEditWidth, xRect.bottom - hudEditHeight, hudEditRight, xRect.bottom };

				dc.ctx->DrawBitmap(Latite::getAssets().hudEditIcon.getBitmap(), hudEditRect);

				if (shouldSelect(hudEditRect, cursorPos)) {
					tooltip = L"Open the HUD editor";
					if (justClicked[0]) {
						playClickSound();
						close();
						Latite::getScreenManager().showScreen("HUDEditor", true);
					}
				}
			}

			// settings button
			RectF settingsRect;
			{
				float setSize = rect.getWidth() * 0.02745f;
				float right = hudEditRect.left - betw;
				settingsRect = { right - setSize, hudEditRect.bottom - setSize, right, hudEditRect.bottom };

				if (shouldSelect(settingsRect, cursorPos)) {
					tooltip = L"Open general client settings";
					if (justClicked[0]) {
						playClickSound();
						this->tab = SETTINGS;
					}
				}

				dc.ctx->DrawBitmap(Latite::getAssets().cogIcon.getBitmap(), settingsRect);
			}
		}
	}

	// Search Bar + tabs 
	RectF searchRect{};
	{
		float gaps = guiWidth * 0.02217f;
		float gapY = rect.getHeight() * 0.0175f;

		// prototype height = 564

		float searchWidth = guiWidth * 0.25f;
		float searchHeight = 0.0425f * rect.getHeight();
		float searchRound = searchHeight * 0.416f;

		searchRect = { logoRect.left, logoRect.bottom + gapY, logoRect.left + searchWidth, logoRect.bottom + gapY + searchHeight };
		auto searchCol = d2d::Color::RGB(0x70, 0x70, 0x70).asAlpha(0.28f);

		if (shouldSelect(searchRect, cursorPos)) {
			cursor = Cursor::IBeam;
			shouldArrow = false;
			
		}

		if (justClicked[0]) {
			if (shouldSelect(searchRect, cursorPos))
				searchTextBox.setSelected(true);
			else searchTextBox.setSelected(false);
		}

		{

			dc.ctx->SetTarget(shadowBitmap.Get());
			dc.ctx->Clear();

			dc.ctx->SetTransform(currentMatr);

			D2D1_ROUNDED_RECT rr;
			rr.radiusX = searchRound;
			rr.radiusY = searchRound;
			rr.rect = searchRect.get();
			rend.getSolidBrush()->SetColor(searchCol.get());
			dc.ctx->FillRoundedRectangle(rr, rend.getSolidBrush());

			// Shadow

			D2D1::Matrix3x2F matr = D2D1::Matrix3x2F::Translation(5 * adaptedScale, 5 * adaptedScale);
			affineTransformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matr);
			shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::Vector4F(0.f, 0.f, 0.f, 0.4f));
			shadowEffect->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, D2D1_SHADOW_OPTIMIZATION_SPEED);

			shadowEffect->SetInput(0, shadowBitmap.Get());
			compositeEffect->SetInputEffect(0, affineTransformEffect);
			compositeEffect->SetInput(1, shadowBitmap.Get());
			{
				std::wstring searchStr = L"";
				if (searchTextBox.getText().empty() && !searchTextBox.isSelected()) {
					if (this->tab == SETTINGS) {
						searchStr += L"Search Settings";
					}
					else if (this->tab == MODULES) {
						searchStr += L"Search";
					}
				}
				else {
					searchStr = util::StrToWStr(searchTextBox.getText());
				}
				Vec2 ts = dc.getTextSize(util::StrToWStr(searchTextBox.getText().substr(0, searchTextBox.getCaretLocation())), Renderer::FontSelection::Regular, searchRect.getHeight() / 2.f);
				RectF textSearchRect = { searchRect.left + 5.f + searchRect.getHeight(), searchRect.top, searchRect.right - 5.f + searchRect.getHeight(), searchRect.bottom };
				d2d::Rect blinkerRect = { textSearchRect.left + ts.x, searchRect.top + 3.f, textSearchRect.left + ts.x + 2.f, searchRect.bottom - 3.f };
				if (searchTextBox.isSelected() && searchTextBox.shouldBlink() ) dc.fillRectangle(blinkerRect, d2d::Color::RGB(0xB9, 0xB9, 0xB9));
				dc.drawText(textSearchRect, searchStr, d2d::Color::RGB(0xB9, 0xB9, 0xB9), FontSelection::Regular, searchRect.getHeight() / 2.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				dc.ctx->DrawBitmap(Latite::getAssets().searchIcon.getBitmap(), { searchRect.left + 10.f, searchRect.top + 6.f, searchRect.left - 3.f + searchRect.getHeight(), searchRect.top + searchRect.getHeight() - 6.f });
			}

			dc.ctx->SetTarget(myBitmap);
		}

		if (tab == SETTINGS) {
			// actual settings
			auto& settings = Latite::getSettings();

			float settingWidth = rect.getWidth() / 3.f;
			float padToSettings = 0.04787f * rect.getHeight();
			// float settings
			Vec2 setPos = { searchRect.left, searchRect.bottom + padToSettings };
			{
				// go through all float settings
				settings.forEach([&](std::shared_ptr<Setting> set) {
					if (set->value->index() == (size_t)Setting::Type::Float /* || set->value->index() == Setting::Type::Int*/) {
						drawSetting(set.get(), setPos, dc, settingWidth, 0.35f);
						setPos.y += (setting_height_relative * rect.getHeight()) * 3.f;
					}
					});
			}

			// key/enum settings
			setPos.y += padToSettings;
			{
				// go through all enum settings
				settings.forEach([&](std::shared_ptr<Setting> set) {
					if (set->value->index() == (size_t)Setting::Type::Key /* || set->value->index() == Setting::Type::Enum*/) {
						drawSetting(set.get(), setPos, dc, settingWidth);
						setPos.y += (setting_height_relative * rect.getHeight()) * 3.f;
					}
					});
			}

			// bool settings
			setPos = { rect.left + rect.getWidth() * ( 1.3f / 3.f ), searchRect.bottom + padToSettings };
			{
				// go through all bool settings
				settings.forEach([&](std::shared_ptr<Setting> set) {
					if (set->value->index() == (size_t)Setting::Type::Bool /* || set->value->index() == Setting::Type::Enum*/) {
						drawSetting(set.get(), setPos, dc, settingWidth);
						setPos.y += (setting_height_relative * rect.getHeight()) * 3.f;
					}
					});
			}
		} else if (tab == MODULES) {

			// all, game, hud, etc buttons
			static std::vector<std::tuple<std::wstring, ClickGUI::ModTab, d2d::Color>> modTabs = { {L"All", ALL, searchCol }, {L"Game", GAME, searchCol}, {L"HUD", HUD, searchCol }, { L"Script", SCRIPT, searchCol } };

			float nodeWidth = guiWidth * 0.083f;

			RectF nodeRect = { searchRect.right + gaps, searchRect.top, searchRect.right + gaps + nodeWidth, searchRect.bottom };

			for (auto& pair : modTabs) {
				bool contains = shouldSelect(nodeRect, cursorPos);
				std::get<2>(pair) = util::LerpColorState(std::get<2>(pair), searchCol + 0.2f, searchCol, contains);

				if (justClicked[0] && contains) {
					this->modTab = std::get<1>(pair);
					playClickSound();
				}

				dc.ctx->SetTarget(shadowBitmap.Get());
				D2D1_ROUNDED_RECT rr;
				rr.radiusX = searchRound;
				rr.radiusY = searchRound;
				rr.rect = nodeRect.get();
				auto solidBrush = rend.getSolidBrush();
				solidBrush->SetColor(std::get<2>(pair).get());
				dc.ctx->FillRoundedRectangle(rr, rend.getSolidBrush());

				dc.drawText(nodeRect, std::get<0>(pair), {1.f, 1.f, 1.f, 0.8f}, FontSelection::Regular, nodeRect.getHeight() / 2.f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				dc.ctx->SetTarget(myBitmap);

				auto oWidth = nodeRect.getWidth() + gaps;
				nodeRect.left += oWidth;
				nodeRect.right += oWidth;

			}
		}
		dc.ctx->SetTransform(oTransform);
		dc.ctx->DrawImage(compositeEffect.Get());
		dc.ctx->SetTransform(currentMatr);
	}

	// Panels
	if (this->tab == MODULES) {
		auto modulePad = guiWidth * 0.0317f;
		int numMods = 3;
		float modBetwPad = modulePad / 2.f;
		float totalPad = (modBetwPad * 2.f) + modulePad * 2.f;
		float modWidth = (guiWidth - totalPad) / numMods;
		float modHeight = 0.08F * rect.getHeight();
		float padFromSearchBar = 0.034F * rect.getHeight();

		float xStart = rect.left + modulePad;
		float x = xStart;
		float y = searchRect.bottom + padFromSearchBar;
		// TODO: clipping
		dc.ctx->PushAxisAlignedClip({ rect.left, y, rect.right, rect.bottom }, D2D1_ANTIALIAS_MODE_ALIASED);

		// Sort Modules
		static std::vector<ModContainer> mods = {};

		static size_t lastCount = 0;
		if (mods.empty() || (Latite::getModuleManager().size() != lastCount)) {
			lastCount = Latite::getModuleManager().size();
			mods.clear();
			Latite::getModuleManager().forEach([&](std::shared_ptr<IModule> mod) {
				if (mod->isVisible()) {
					ModContainer container{ mod->getDisplayName(), mod->desc(), mod };
					mods.emplace_back(container);
				}
				return false;
				});
		}

		std::sort(mods.begin(), mods.end(), ModContainer::compare); // Sort modules

		//std::array<float, 3> 

		for (auto& mod : mods) {
			mod.shouldRender = true;

			if (modTab == GAME && mod.mod->getCategory() == IModule::HUD) mod.shouldRender = false; // Game Tab
			if (modTab == HUD && !mod.mod->isHud()) mod.shouldRender = false; // Hud Tab
			if (modTab == SCRIPT && mod.mod->getCategory() != IModule::SCRIPT) mod.shouldRender = false; // Hud Tab

			bool should = mod.shouldRender;
			if (this->searchTextBox.getText().size() > 0) {
				should = false;
				std::string lower = mod.name;
				std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
				std::string lowerSearch = searchTextBox.getText();
				std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
				if (lower.rfind(lowerSearch) != UINTPTR_MAX) should = true;
			}
			mod.shouldRender = should;
		}

		int i = 0;
		int row = 1;
		int column = 1;

		std::array<float, 3> columnOffs = { 0.f, 0.f, 0.f };

		modClip = true;

		// modules

		for (auto& mod : mods) {
			if (!mod.shouldRender) continue;
			Vec2 pos = { x, y + columnOffs[i]};
			RectF modRect = { pos.x, pos.y, pos.x + modWidth, pos.y + modHeight };
			RectF modRectActual = modRect;

			float textHeight = 0.4f * modHeight;
			float rlBounds = modWidth * 0.04561f;

			// module settings calculations
			if (mod.isExtended) {

				// clipping pane
				{
					dc.ctx->SetTarget(shadowBitmap.Get());
					dc.ctx->Clear();

					float textSizeDesc = textHeight * 0.72f;
					float descTextPad = textSizeDesc / 3.f;
					RectF descTextRect = { modRect.left + rlBounds, modRect.bottom, modRect.right - rlBounds, modRect.bottom + textSizeDesc + descTextPad };
					modRectActual.bottom = descTextRect.bottom;

					dc.drawText(descTextRect, util::StrToWStr(mod.description), d2d::Color(1.f, 1.f, 1.f, 0.57f), FontSelection::Regular, textSizeDesc, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

					float padToSetting = 0.014184f * rect.getHeight();
					float settingPadY = padToSetting * 2.f;
					float settingHeight = rect.getHeight() * setting_height_relative;

					modRectActual.bottom += padToSetting;
					mod.mod->settings->forEach([&](std::shared_ptr<Setting> set) {
						if (set->visible) {
							if (drawSetting(set.get(), { descTextRect.left, modRectActual.bottom }, dc, descTextRect.getWidth(), 0.25f)) {
								modRectActual.bottom += settingHeight;
								modRectActual.bottom += settingPadY;
							}
						}
						});

					dc.ctx->SetTarget(myBitmap);
				}
			}

			dc.fillRoundedRectangle(modRectActual, d2d::Color::RGB(0x44, 0x44, 0x44).asAlpha(0.22f), .22f * modHeight);
			if (mod.mod->isEnabled()) dc.drawRoundedRectangle(modRectActual, d2d::Color::RGB(0x32, 0x39, 0x76).asAlpha(1.f), .22f * modHeight, 1.f, DXContext::OutlinePosition::Inside);;
			if (mod.isExtended) {
				dc.ctx->DrawBitmap(shadowBitmap.Get());
			}

			float togglePad = modHeight * 0.249f;
			float toggleWidth = modWidth * 0.143f;
			RectF toggleRect = { modRect.right - togglePad - toggleWidth, modRect.top + togglePad,
			modRect.right - togglePad, modRect.bottom - togglePad };

			// text
			auto textRect = modRect;
			textRect.left += modRect.getWidth() / 6.f;
			dc.drawText(textRect, util::StrToWStr(mod.name), { 1.f, 1.f, 1.f, 1.f }, FontSelection::Light, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

			// toggle
			{

				bool selecToggle;
				if (selecToggle = this->shouldSelect(toggleRect, cursorPos)) {
					if (justClicked[0]) {
						mod.mod->setEnabled(!mod.mod->isEnabled());
						playClickSound();
					}
				}
				static auto onCol = d2d::Color::RGB(0x32, 0x39, 0x76);
				static auto offCol = d2d::Color::RGB(0x63, 0x63, 0x63);

				mod.toggleColorOn = util::LerpColorState(mod.toggleColorOn, onCol + 0.2f, onCol, selecToggle);
				mod.toggleColorOff = util::LerpColorState(mod.toggleColorOff, offCol + 0.2f, offCol, selecToggle);

				//float aTogglePadY = toggleRect.getHeight() * 0.15f;
				float radius = toggleRect.getHeight() * 0.35f;
				float circleOffs = toggleWidth * 0.27f;

				dc.fillRoundedRectangle(toggleRect, mod.mod->isEnabled() ? mod.toggleColorOn : mod.toggleColorOff, toggleRect.getHeight() / 2.f);
				Vec2 center{ toggleRect.left + circleOffs, toggleRect.centerY() };
				Vec2 center2 = center;
				center2.x = toggleRect.right - circleOffs;
				float onDist = center2.x - center.x;

				mod.lerpToggle = std::lerp(mod.lerpToggle, mod.mod->isEnabled() ? 1.f : 0.f, sdk::ClientInstance::get()->minecraft->timer->alpha * 0.3f);

				center.x += onDist * mod.lerpToggle;

				dc.brush->SetColor((d2d::Color(0xB9, 0xB9, 0xB9)).get());
				dc.ctx->FillEllipse(D2D1::Ellipse({ center.x, center.y }, radius, radius), dc.brush);
			}


			// arrow
			{
				RectF rc = { modRect.left + (modRect.getHeight() * 0.4f),
					modRect.top + (modRect.getHeight() * 0.4f), modRect.left + modRect.getHeight() * 0.70f, modRect.bottom - modRect.getHeight() * 0.4f };

				if (this->shouldSelect(modRect, cursorPos) && !shouldSelect(toggleRect, cursorPos)) {
					if (justClicked[0]) {
						mod.isExtended = !mod.isExtended;
						//playClickSound();
					}
				}


				D2D1::Matrix3x2F oMatr;
				dc.ctx->GetTransform(&oMatr);
				float toLerp = mod.isExtended ? 0.f : 180.f;
				dc.ctx->SetTransform(D2D1::Matrix3x2F::Rotation(mod.lerpArrowRot, { rc.centerX(), rc.centerY() }) * oMatr);
				mod.lerpArrowRot = std::lerp(mod.lerpArrowRot, toLerp, sdk::ClientInstance::get()->minecraft->timer->alpha * 0.3f);
				// icon
				dc.ctx->DrawBitmap(Latite::getAssets().arrowIcon.getBitmap(), rc.get());
				dc.ctx->SetTransform(oMatr);
			}

			columnOffs[i] += modRectActual.getHeight() - modRect.getHeight();

			if (mod.isExtended && mod.mod->isHud() && (modRectActual.contains(cursorPos) || colorPicker.setting)) {
				auto hMod = static_cast<HUDModule*>(mod.mod.get());

				D2D1::Matrix3x2F oTrans;
				dc.ctx->GetTransform(&oTrans);
				dc.ctx->SetTransform(D2D1::Matrix3x2F::Scale(hMod->getScale(), hMod->getScale())* D2D1::Matrix3x2F::Translation(hMod->getRect().left, hMod->getRect().top));
				hMod->render(dc, false, isActive());
				dc.ctx->SetTransform(oTrans);
			}

			if (i >= (numMods - 1)) {
				i = 0;
				row++;
				column = 0;
				y += modRect.getHeight() + padFromSearchBar;
				x = xStart;
				continue;
			}
			else {
				x += modBetwPad + modWidth;
				column++;
			}
			i++;
		}

		dc.ctx->PopAxisAlignedClip();
	}

	modClip = false;

	if (colorPicker.setting) {
		drawColorPicker();
		if (colorPicker.queueClose) {
			auto& colVal = std::get<ColorValue>(*colorPicker.setting->value);
			auto d2dCol = d2d::Color(util::HSVToColor(colorPicker.pickerColor)).asAlpha(colorPicker.opacityMod);
			*colorPicker.selectedColor = { d2dCol.r, d2dCol.g, d2dCol.b, d2dCol.a };
			colorPicker = ColorPicker();
		}
	}
	this->clearLayers();

	dc.ctx->SetTransform(oTransform);
	if (shouldArrow) cursor = Cursor::Arrow;
}

void ClickGUI::onInit(Event&) {
	auto myBitmap = Latite::getRenderer().getBitmap();
	D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
	D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();

	auto dc = Latite::getRenderer().getDeviceContext();

	dc->CreateBitmap(bitmapSize, nullptr, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, pixelFormat), shadowBitmap.GetAddressOf());
	dc->CreateEffect(CLSID_D2D1Composite, compositeEffect.GetAddressOf());
	
}

void ClickGUI::onCleanup(Event&) {
	compositeEffect = nullptr;
}


void ClickGUI::onKey(Event& evGeneric) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
	if (searchTextBox.isSelected()) {
		ev.setCancelled(true);
		return;
	}
	if (this->activeSetting) {
		if (ev.isDown()) {
			if (ev.getKey() == VK_ESCAPE) {
				activeSetting = nullptr;
				ev.setCancelled(true);
				return;
			}
			else {
				this->capturedKey = ev.getKey();
			}
		}
	}

	if (ev.isDown() && ev.getKey() == VK_ESCAPE) {
		if (colorPicker.setting) {
			colorPicker.queueClose = true;
		}
		else {
			this->close();
		}
	}

	ev.setCancelled(true);
}

void ClickGUI::onClick(Event& evGeneric) {
	auto& ev = reinterpret_cast<ClickEvent&>(evGeneric);
	if (ev.getMouseButton() > 0) {
		ev.setCancelled(true);
	}
}


namespace {
	void drawAlphaBar(DXContext& dc, d2d::Rect rect, float nodeSize, int rows) {
		float endY = rect.top;
		endY += rect.getHeight() / rows;
		float beginY = rect.top;
		// gray part
		float bs = nodeSize;

		for (int i = 0; i < rows; i++) {
			if (i % 2 == 0) {
				for (float beginX = rect.left; beginX < rect.right; beginX += bs * 2.f) {
					float endX = std::min(rect.right, beginX + bs);
					dc.fillRectangle({ beginX, beginY, endX, endY }, { 1.f, 1.f, 1.f, 0.5f });

				}
			}
			else {
				for (float beginX = rect.left + bs; beginX < rect.right; beginX += bs * 2.f) {
					float endX = std::min(rect.right, beginX + bs);
					dc.fillRectangle({ beginX, beginY, endX, endY }, { 1.f, 1.f, 1.f, 0.5f });
				}
			}
			beginY = endY;
			endY += rect.getHeight() / rows;
		}
	}
}

bool ClickGUI::drawSetting(Setting* set, Vec2 const& pos, DXContext& dc, float size, float fTextWidth) {
	const float checkboxSize = rect.getWidth() * setting_height_relative;
	const float textSize = checkboxSize * 0.9f;
	const auto cursorPos = sdk::ClientInstance::get()->cursorPos;
	const float round = 0.1875f * checkboxSize;

	switch ((Setting::Type)((*set->value).index())) {
	case Setting::Type::Bool:
	{
		RectF checkboxRect = { pos.x, pos.y, pos.x + checkboxSize, pos.y + checkboxSize };
		bool contains = this->shouldSelect(checkboxRect, cursorPos);

		auto colOff = d2d::Color::RGB(0xD9, 0xD9, 0xD9).asAlpha(0.11f);
		if (!set->rendererInfo.init) {
			set->rendererInfo.init = true;
			set->rendererInfo.col[0] = colOff.r;
			set->rendererInfo.col[1] = colOff.g;
			set->rendererInfo.col[2] = colOff.b;
			set->rendererInfo.col[3] = colOff.a;
		}
		auto lerpedColor = util::LerpColorState(set->rendererInfo.col, colOff + 0.1f, colOff, contains);
		set->rendererInfo.col[0] = lerpedColor.r;
		set->rendererInfo.col[1] = lerpedColor.g;
		set->rendererInfo.col[2] = lerpedColor.b;
		set->rendererInfo.col[3] = lerpedColor.a;

		if (contains && justClicked[0]) {
			std::get<BoolValue>(*set->value) = !std::get<BoolValue>(*set->value);
			playClickSound();
		}


		dc.fillRoundedRectangle(checkboxRect, Color(set->rendererInfo.col), round);
		if (std::get<BoolValue>(*set->value)) {
			float checkWidth = 0.6f * checkboxSize;
			float checkHeight = 0.375f * checkboxSize;
			RectF markRect = { checkboxRect.left + checkWidth / 4.f, checkboxRect.top + checkHeight / 2.f,
			checkboxRect.right - checkWidth / 4.f, checkboxRect.bottom - checkHeight / 2.f };

			dc.ctx->DrawBitmap(Latite::getAssets().checkmarkIcon.getBitmap(), markRect);
		}
		float offs = checkboxSize * 0.66f;

		float newX = checkboxRect.right + offs;
		float rem = newX - pos.x;
		RectF textRect = { newX, checkboxRect.top, newX + (size - rem), checkboxRect.bottom };
		auto disp = util::StrToWStr(set->getDisplayName());

		dc.drawText(textRect, disp, { 1.f, 1.f, 1.f, 1.f }, FontSelection::Regular, textSize, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		auto desc = util::StrToWStr(set->desc());
		if (!desc.empty())
			if (contains || shouldSelect(textRect, cursorPos)) tooltip = desc;
	}
		break;
	case Setting::Type::Key:
	{
		RectF keyRect = { pos.x, pos.y, pos.x + checkboxSize * 2.f, pos.y + checkboxSize };
		bool contains = this->shouldSelect(keyRect, cursorPos);

		auto colOff = d2d::Color::RGB(0xD9, 0xD9, 0xD9).asAlpha(0.11f);
		if (!set->rendererInfo.init) {
			set->rendererInfo.init = true;
			set->rendererInfo.col[0] = colOff.r;
			set->rendererInfo.col[1] = colOff.g;
			set->rendererInfo.col[2] = colOff.b;
			set->rendererInfo.col[3] = colOff.a;
		}
		auto lerpedColor = util::LerpColorState(set->rendererInfo.col, colOff + 0.1f, colOff, contains);
		set->rendererInfo.col[0] = lerpedColor.r;
		set->rendererInfo.col[1] = lerpedColor.g;
		set->rendererInfo.col[2] = lerpedColor.b;
		set->rendererInfo.col[3] = lerpedColor.a;

		std::wstring text = util::StrToWStr(util::KeyToString(std::get<KeyValue>(*set->value)));

		if (set == activeSetting) {
			if (justClicked[0] && !contains) {
				activeSetting = nullptr;
			}
		}

		// white outline
		if (set == activeSetting) {
			text = L"...";
		}

		auto ts = dc.getTextSize(text, FontSelection::Regular, textSize * 0.9f) + Vec2(8.f, 0.f);
		if (ts.x > keyRect.getWidth()) keyRect.right = keyRect.left + (ts.x);

		
		dc.fillRoundedRectangle(keyRect, Color(set->rendererInfo.col), round);

		dc.drawText(keyRect, text, d2d::Color(1.f, 1.f, 1.f, 1.f), FontSelection::Regular, textSize * 0.9f,
			DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		if (activeSetting == set) {
			dc.drawRoundedRectangle(keyRect, d2d::Color(1.f, 1.f, 1.f, 1.f), round);
		}
		if (activeSetting == set && this->capturedKey > 0) {
			std::get<KeyValue>(*set->value) = capturedKey;
			activeSetting = 0;
			capturedKey = 0;
		}
		

		float padToName = 0.006335f * rect.getWidth();
		float newX = keyRect.right + padToName;
		float rem = newX - pos.x;


		RectF textRect = { keyRect.right + padToName, keyRect.top, newX + (size - rem), keyRect.bottom };

		auto disp = util::StrToWStr(set->getDisplayName());
		dc.drawText(textRect, disp, { 1.f, 1.f, 1.f, 1.f }, FontSelection::Regular, textSize, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		if (!set->desc().empty())
			if (shouldSelect(textRect, cursorPos)) tooltip = util::StrToWStr(set->desc());
		if (shouldSelect(keyRect, cursorPos)) {
			tooltip = L"Right click to reset";
			if (justClicked[0]) {
				if (!this->activeSetting) activeSetting = set;
				playClickSound();
			}
			if (justClicked[1]) {
				activeSetting = nullptr;
				std::get<KeyValue>(*set->value) = 0;
				playClickSound();
			}
		}
	}
		break;
	case Setting::Type::Enum:
	{
		RectF enumRect = { pos.x, pos.y, pos.x + checkboxSize * 2.f, pos.y + checkboxSize };
		bool contains = this->shouldSelect(enumRect, cursorPos);

		EnumValue& val = std::get<EnumValue>(*set->value);

		auto colOff = d2d::Color::RGB(0xD9, 0xD9, 0xD9).asAlpha(0.11f);
		if (!set->rendererInfo.init) {
			set->rendererInfo.init = true;
			set->rendererInfo.col[0] = colOff.r;
			set->rendererInfo.col[1] = colOff.g;
			set->rendererInfo.col[2] = colOff.b;
			set->rendererInfo.col[3] = colOff.a;
		}
		auto lerpedColor = util::LerpColorState(set->rendererInfo.col, colOff + 0.1f, colOff, contains);
		set->rendererInfo.col[0] = lerpedColor.r;
		set->rendererInfo.col[1] = lerpedColor.g;
		set->rendererInfo.col[2] = lerpedColor.b;
		set->rendererInfo.col[3] = lerpedColor.a;

		auto text = util::StrToWStr(set->enumData->getSelectedName());

		auto ts = dc.getTextSize(text, FontSelection::Regular, textSize * 0.9f) + Vec2(8.f, 0.f);
		if (ts.x > enumRect.getWidth()) enumRect.right = enumRect.left + (ts.x);

		dc.fillRoundedRectangle(enumRect, Color(set->rendererInfo.col), round);

		dc.drawText(enumRect, text, d2d::Color(1.f, 1.f, 1.f, 1.f), FontSelection::Regular, textSize * 0.9f,
			DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		if (activeSetting == set) {
			dc.drawRoundedRectangle(enumRect, d2d::Color(1.f, 1.f, 1.f, 1.f), round);
		}


		float padToName = 0.006335f * rect.getWidth();
		float newX = enumRect.right + padToName;
		float rem = newX - pos.x;


		RectF textRect = { enumRect.right + padToName, enumRect.top, newX + (size - rem), enumRect.bottom };

		dc.drawText(textRect, util::StrToWStr(set->getDisplayName()), {1.f, 1.f, 1.f, 1.f}, FontSelection::Regular, textSize, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		if (!set->desc().empty())
			if (shouldSelect(textRect, cursorPos)) tooltip = util::StrToWStr(set->desc());

		if (shouldSelect(enumRect, cursorPos)) {
			if (set->enumData->getSelectedDesc().size() > 0) {
				tooltip = util::StrToWStr(set->enumData->getSelectedDesc());
			}
			else tooltip = util::StrToWStr(set->enumData->getSelectedName());
		}

		if (shouldSelect(enumRect, cursorPos)) {
			if (justClicked[0]) {
				// cycle
				set->enumData->next();
				playClickSound();
			}
		}
		break;
	}
	case Setting::Type::Color:
	{
		float padToName = 0.006335f * rect.getWidth();

		RectF colRect = { pos.x, pos.y, pos.x + checkboxSize * 2.f, pos.y + checkboxSize };
		bool contains = this->shouldSelect(colRect, cursorPos);
		std::wstring name = util::StrToWStr(set->getDisplayName());

		auto& colVal = std::get<ColorValue>(*set->value);

		RectF textRect = { colRect.right + padToName, colRect.top, pos.x + size, colRect.bottom };
		dc.drawText(textRect, name, { 1.f, 1.f, 1.f, 1.f }, FontSelection::Regular, textSize, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		
		ComPtr<ID2D1LinearGradientBrush> gradientBrush;
		ComPtr<ID2D1GradientStopCollection> gradientStopCollection;

		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES prop{};
		auto ss = Latite::getRenderer().getScreenSize();
		prop.startPoint = { 0.f, ss.height / 2.f };
		prop.endPoint = { ss.width, ss.height / 2.f };

		d2d::Color col = { colVal.color1.r, colVal.color1.g, colVal.color1.b, colVal.color1.a };

		const D2D1_GRADIENT_STOP stops[] = {
			0.f, col.asAlpha(1.f).get(),
			1.f, col.get()
		};
		
		dc.ctx->CreateGradientStopCollection(stops, _countof(stops), gradientStopCollection.GetAddressOf());
		dc.ctx->CreateLinearGradientBrush(prop, gradientStopCollection.Get(), gradientBrush.GetAddressOf());
		
		gradientBrush->SetStartPoint({ colRect.left, textRect.centerY() });
		gradientBrush->SetEndPoint({ colRect.right, textRect.centerY() });
		ComPtr<ID2D1GradientStopCollection> stopCol;

		dc.fillRoundedRectangle(colRect, { 1.f, 1.f, 1.f, 0.4f }, round);
		// alpha bar

		float apad = 1.f;
		dc.ctx->PushAxisAlignedClip({ colRect.left + apad, colRect.top + apad, colRect.right - apad, colRect.bottom - apad }, D2D1_ANTIALIAS_MODE_ALIASED);
		drawAlphaBar(dc, colRect, colRect.getWidth() / 16.f, 11);
		dc.ctx->PopAxisAlignedClip();

		dc.fillRoundedRectangle(colRect, gradientBrush.Get(), round);
		dc.drawRoundedRectangle(colRect, gradientBrush.Get(), round, 1.f, DXContext::OutlinePosition::Inside);
		
		if (shouldSelect(colRect, cursorPos)) {
			if (justClicked[0]) {
				playClickSound();
				colorPicker.setting = set;
				colorPicker.dragging = false;
				cPickerRect = { colRect.left, colRect.bottom + 20.f, 0.f, 0.f };
				auto& colVal = std::get<ColorValue>(*set->value);
				colorPicker.selectedColor = &colVal.color1;
				auto sCol = *colorPicker.selectedColor;
				colorPicker.pickerColor = util::ColorToHSV({ sCol.r, sCol.g, sCol.b, sCol.a });
				colorPicker.hueMod = colorPicker.pickerColor.h / 360.f;
				colorPicker.svModX = colorPicker.pickerColor.s;
				colorPicker.svModY = 1.f - colorPicker.pickerColor.v;
				colorPicker.opacityMod = sCol.a;
			}
		}
	}
	break;
	case Setting::Type::Float:
	{
		float textWidth = fTextWidth * size;
		float sliderHeight = (rect.getHeight() * 0.017730f);

		float textSz = sliderHeight * 1.5f;

		RectF textRect = { pos.x, pos.y, pos.x + textWidth, pos.y + sliderHeight };
		RectF rtTextRect = textRect.translate(0.f, -(textRect.getHeight() / 2.f));
		std::wstringstream namew;
		namew << util::StrToWStr(set->getDisplayName());
		dc.drawText(rtTextRect, namew.str(), d2d::Color(1.f, 1.f, 1.f, 1.f), FontSelection::Semilight, textSz, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		float padToSlider = rect.getHeight() * 0.01063f;

		float sliderTop = textRect.top + sliderHeight * 0.16f;
		RectF sliderRect = { textRect.right, sliderTop, textRect.left + size - (0.1947f * size), sliderTop + sliderHeight };
		
		float innerPad = 0.2f * sliderRect.getHeight();
		RectF innerSliderRect = { sliderRect.left + innerPad, sliderRect.top + innerPad, sliderRect.right - innerPad, sliderTop + (rect.getHeight() * 0.017730f) - innerPad };

		std::wstringstream valuew;
		valuew << std::fixed << std::setprecision(2) << std::get<FloatValue>(*set->value);

		RectF rightRect = { sliderRect.right, sliderRect.top, pos.x + size, sliderRect.bottom };
		RectF rtRect = rightRect.translate(0.f, -(sliderRect.getHeight() / 2.f));
		dc.drawText(rtRect, valuew.str(), d2d::Color(1.f, 1.f, 1.f, 1.f), Renderer::FontSelection::Semilight, sliderHeight * 1.4f, DWRITE_TEXT_ALIGNMENT_CENTER);

		float min = std::get<FloatValue>(set->min);
		float max = std::get<FloatValue>(set->max);
		float interval = std::get<FloatValue>(set->interval);

		if (!this->activeSetting) {
			if (justClicked[0] && shouldSelect(sliderRect, cursorPos)) {
				activeSetting = set;
				playClickSound();
			}
		}
		else {
			if (activeSetting == set) {
				if (!mouseButtons[0]) activeSetting = nullptr;

				float find = (cursorPos.x - sliderRect.left) / sliderRect.getWidth();

				float percent = (find);

				float newVal = percent * (std::get<FloatValue>(set->max) - min);
				newVal += min;

				newVal = std::clamp(newVal, min, max);

				// Find a good value to set to ("latch to nearest")
				newVal /= interval;
				newVal = std::round(newVal);
				newVal *= interval;

				std::get<FloatValue>(*set->value) = newVal;
			}
		}

		float percent = std::get<FloatValue>(*set->value) / max;
		float oRight = innerSliderRect.right;
		float oLeft = innerSliderRect.left;
		float newRight = 0.f;

		if (activeSetting == set) {
			newRight = cursorPos.x;
		}
		else {
			newRight = sliderRect.left + (sliderRect.getWidth() * percent);
		}
		innerSliderRect.right = std::clamp(newRight, oLeft, oRight);

		dc.fillRoundedRectangle(sliderRect, d2d::Color::RGB(0x8D, 0x8D, 0x8D).asAlpha(0.11f), sliderRect.getHeight() / 2.f);
		// 323976
		dc.fillRoundedRectangle(innerSliderRect, d2d::Color::RGB(0x32, 0x39, 0x76), innerSliderRect.getHeight() / 2.f);

		dc.brush->SetColor(d2d::Color(0xB9, 0xB9, 0xB9).get());
		dc.ctx->FillEllipse(D2D1::Ellipse({ innerSliderRect.right, sliderRect.centerY() }, sliderRect.getHeight() * 0.6f, sliderRect.getHeight() * 0.6f), dc.brush);
	}
		break;
	default:
		return false;
	}
	return true;
}

bool ClickGUI::shouldSelect(d2d::Rect rc, Vec2 const& pt) {
	if (modClip) {
		if (!rect.contains(pt) || !Screen::shouldSelect(rc, pt)) {
			return false;
		}
	}
	return Screen::shouldSelect(rc, pt);
}

void ClickGUI::drawColorPicker() {
	auto& cursorPos = sdk::ClientInstance::get()->cursorPos;
	DXContext dc;
	dc.ctx->SetTarget(shadowBitmap.Get());
	dc.ctx->Clear();

	float rectWidth = 0.2419f * rect.getWidth();
	cPickerRect.right = cPickerRect.left + rectWidth;

	float boxWidth = 0.79f * rectWidth;
	float remPad = (rectWidth - boxWidth) / 2.f;

	// Color PIcker Text
	float textSize = 0.09f * rectWidth;
	RectF titleRect = { cPickerRect.left + remPad, cPickerRect.top + remPad, cPickerRect.right - remPad, cPickerRect.top + remPad + textSize };

	{
		dc.drawText(titleRect, L"Color Picker", { 1.f, 1.f, 1.f, 1.f }, Renderer::FontSelection::Light, textSize, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	float boxTop = titleRect.bottom + remPad;

	RectF boxRect = { titleRect.left, boxTop, titleRect.right, boxTop + boxWidth };

	ComPtr<ID2D1LinearGradientBrush> mainColorBrush;
	ComPtr<ID2D1LinearGradientBrush> valueBrush;
	ComPtr<ID2D1LinearGradientBrush> hueBrush;
	ComPtr<ID2D1LinearGradientBrush> alphaBrush;

	// TODO: support chroma, multiple colors
	auto& colVal = std::get<ColorValue>(*colorPicker.setting->value);
	d2d::Color col = util::HSVToColor(colorPicker.pickerColor);
	d2d::Color sCol = { colorPicker.selectedColor->r, colorPicker.selectedColor->g, colorPicker.selectedColor->b, colorPicker.selectedColor->a };
	d2d::Color nsCol = util::HSVToColor({ util::ColorToHSV(sCol).h, 1.f, 1.f });
	d2d::Color baseCol = util::HSVToColor({ colorPicker.pickerColor.h, 1.f, 1.f });

	// main brush
	{
		ComPtr<ID2D1GradientStopCollection> gradientStopCollection;

		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES prop{};
		auto ss = Latite::getRenderer().getScreenSize();
		prop.startPoint = { boxRect.left, boxRect.top };
		prop.endPoint = { boxRect.right, boxRect.top };

		const D2D1_GRADIENT_STOP stops[] = {
			0.f, { 1.f, 1.f, 1.f, 1.f },
			1.f, baseCol.get()
		};

		dc.ctx->CreateGradientStopCollection(stops, _countof(stops), gradientStopCollection.GetAddressOf());
		dc.ctx->CreateLinearGradientBrush(prop, gradientStopCollection.Get(), mainColorBrush.GetAddressOf());
	}

	// Value brush
	{
		ComPtr<ID2D1GradientStopCollection> gradientStopCollection;

		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES prop{};
		auto ss = Latite::getRenderer().getScreenSize();
		prop.startPoint = { boxRect.left, boxRect.bottom };
		prop.endPoint = { boxRect.left, boxRect.top };

		const D2D1_GRADIENT_STOP stops[] = {
			0.f, { 0.f, 0.f, 0.f, 1.f},
			1.f, { 0.f, 0.f, 0.f, 0.f }
		};

		dc.ctx->CreateGradientStopCollection(stops, _countof(stops), gradientStopCollection.GetAddressOf());
		dc.ctx->CreateLinearGradientBrush(prop, gradientStopCollection.Get(), valueBrush.GetAddressOf());
	}
	// Draw inner part of colorpicker

	dc.fillRectangle(boxRect, mainColorBrush.Get());
	dc.fillRectangle(boxRect, valueBrush.Get());
	dc.drawRectangle(boxRect, d2d::Color::RGB(0x50, 0x50, 0x50).asAlpha(0.28f), 2.f);


	float hueBarHeight = boxRect.getHeight() * 0.0506329f;

	float padToHueBar = remPad * 0.6f;

	RectF hueBar = { boxRect.left, boxRect.bottom + padToHueBar, boxRect.right, boxRect.bottom + hueBarHeight + padToHueBar };

	// Hue brush
	{
		ComPtr<ID2D1GradientStopCollection> gradientStopCollection;

		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES prop{};
		auto ss = Latite::getRenderer().getScreenSize();
		prop.startPoint = { hueBar.left, hueBar.top };
		prop.endPoint = { hueBar.right, hueBar.top };

		float hueMod = 0.f;

		const D2D1_GRADIENT_STOP stops[] = {
			0.f, d2d::Color(util::HSVToColor({ 0.f, 1.f, 1.f })).get(),
			1.f / 7.f, d2d::Color(util::HSVToColor({ (1.f / 7.f) * 360.f, 1.f, 1.f })).get(),
			2.f / 7.f, d2d::Color(util::HSVToColor({ (2.f / 7.f) * 360.f, 1.f, 1.f })).get(),
			3.f / 7.f, d2d::Color(util::HSVToColor({ (3.f / 7.f) * 360.f, 1.f, 1.f })).get(),
			4.f / 7.f, d2d::Color(util::HSVToColor({ (4.f / 7.f) * 360.f, 1.f, 1.f })).get(),
			5.f / 7.f, d2d::Color(util::HSVToColor({ (5.f / 7.f) * 360.f, 1.f, 1.f })).get(),
			6.f / 7.f, d2d::Color(util::HSVToColor({ (6.f / 7.f) * 360.f, 1.f, 1.f })).get(),
			1.f, d2d::Color(util::HSVToColor({ 0.f, 1.f, 1.f })).get(),
		};

		dc.ctx->CreateGradientStopCollection(stops, 8, gradientStopCollection.GetAddressOf());
		dc.ctx->CreateLinearGradientBrush(prop, gradientStopCollection.Get(), hueBrush.GetAddressOf());
	}

	dc.fillRoundedRectangle(hueBar, hueBrush.Get(), hueBar.getHeight() / 2.f);
	dc.drawRoundedRectangle(hueBar, d2d::Color::RGB(0x50, 0x50, 0x50).asAlpha(0.28f), hueBar.getHeight() / 2.f, hueBar.getHeight() / 4.f, DXContext::OutlinePosition::Outside);

	RectF alphaBar = { hueBar.left, hueBar.bottom + padToHueBar, hueBar.right, hueBar.bottom + padToHueBar + hueBarHeight };

	// Alpha brush
	{
		ComPtr<ID2D1GradientStopCollection> gradientStopCollection;

		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES prop{};
		auto ss = Latite::getRenderer().getScreenSize();
		prop.startPoint = { alphaBar.left, alphaBar.top };
		prop.endPoint = { alphaBar.right, alphaBar.top };

		float hueMod = 0.f;

		const D2D1_GRADIENT_STOP stops[] = {
			{0.f, col.asAlpha(0.f).get()},
			{1.f, col.asAlpha(1.f).get()}
		};

		dc.ctx->CreateGradientStopCollection(stops, _countof(stops), gradientStopCollection.GetAddressOf());
		dc.ctx->CreateLinearGradientBrush(prop, gradientStopCollection.Get(), alphaBrush.GetAddressOf());
	}

	dc.fillRoundedRectangle(alphaBar, { 1.f, 1.f, 1.f, 0.5f }, alphaBar.getHeight() / 2.f);

	drawAlphaBar(dc, alphaBar, alphaBar.getHeight() / 2.f, 2);
	//dc.fillRoundedRectangle(hueBar, hueBrush.Get(), hueBar.getHeight() / 2.f);
	dc.fillRoundedRectangle(alphaBar, alphaBrush.Get(), alphaBar.getHeight() / 2.f);
	dc.drawRoundedRectangle(alphaBar, d2d::Color::RGB(0x37, 0x37, 0x37).asAlpha(0.88f), alphaBar.getHeight() / 2.f, alphaBar.getHeight() / 3.f, DXContext::OutlinePosition::Outside);

	// color hex edits/displays

	std::array<std::optional<StoredColor>, 3> cols = { colVal.color1, std::nullopt, std::nullopt };

	RectF lastrc = alphaBar;
	for (size_t i = 0; i < cols.size(); ++i) {
		auto& c = cols[i];
		if (c.has_value()) {
			float colorModeWidth = alphaBar.getWidth() / 4.f;
			float hexBoxWidth = alphaBar.getWidth() * 0.617f;
			float boxHeight = alphaBar.getHeight() * 2.f;
			float colorDisplayWidth = boxHeight;

			float pad = (alphaBar.getWidth() - colorModeWidth - hexBoxWidth - colorDisplayWidth) / 3.f;

			RectF totalDisplayRect = lastrc.translate(0.f, padToHueBar);
			totalDisplayRect.bottom = totalDisplayRect.top + boxHeight;
			lastrc = totalDisplayRect;
			RectF colorModeRect = { totalDisplayRect.left, totalDisplayRect.top, totalDisplayRect.left + colorModeWidth, totalDisplayRect.bottom };
			RectF hexBox = { colorModeRect.right + pad, totalDisplayRect.top, colorModeRect.right + pad + hexBoxWidth, totalDisplayRect.bottom };
			RectF colorDisplayRect = { totalDisplayRect.right - pad - colorDisplayWidth, totalDisplayRect.top, totalDisplayRect.right - pad, totalDisplayRect.bottom };

			if (pickerTextBoxes.size() <= i) {
				pickerTextBoxes.insert(pickerTextBoxes.begin() + i, ui::TextBox(hexBox, 7));
				Latite::get().addTextBox(&pickerTextBoxes[i]);
			}
			auto& tb = pickerTextBoxes[i];

			auto bgCol = d2d::Color::RGB(0x50, 0x50, 0x50).asAlpha(0.28f);

			auto round = 0.1875f * colorModeRect.getHeight();

			dc.fillRoundedRectangle(colorModeRect, bgCol, round);
			//dc.fillRoundedRectangle(hexBox, bgCol, round);
			dc.fillRoundedRectangle(colorDisplayRect, col.asAlpha(colorPicker.opacityMod), round);

			std::wstring alphaTxt = util::StrToWStr(std::format("{:.2f}", colorPicker.opacityMod));

			dc.drawText(colorDisplayRect, alphaTxt, (colorPicker.opacityMod < 0.5f || colorPicker.pickerColor.v < 0.5f) ? D2D1::ColorF::White : D2D1::ColorF::Black, Renderer::FontSelection::Regular, colorDisplayRect.getHeight() * 0.5f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			tb.setRect(hexBox);

			if (!tb.isSelected()) {
				tb.setText("#" + col.getHex());
			}

			tb.render(dc, round, bgCol, D2D1::ColorF::White);
			if (tb.isSelected()) {
				d2d::Color newCol = col;
				std::string txt = tb.getText();
				if (txt[0] == '#') {
					txt = txt.substr(1);
				}
				if (txt.size() == 6)
				try {
					newCol = d2d::Color::Hex(txt);
				}
				catch (...) {
				}
				
				auto newHSV = util::ColorToHSV(newCol);
				colorPicker.svModX = newHSV.s;
				colorPicker.svModY = 1.f - newHSV.v;
				colorPicker.hueMod = newHSV.h / 360.f;
			}
			else {
				tb.setCaretLocation(static_cast<int>(tb.getText().size()));
			}

			if (justClicked[0]) {
				tb.setSelected(hexBox.contains(cursorPos));
			}
		}
	}

	float ellipseRadius = 0.75f * alphaBar.getHeight();
	

	// sv
	if (colorPicker.isEditingSV || (justClicked[0] && boxRect.contains(cursorPos))) {
		colorPicker.svModX = std::max(std::min(cursorPos.x - boxRect.left, boxRect.getWidth()) / boxRect.getWidth(), 0.f);
		colorPicker.svModY = std::max(std::min(cursorPos.y - boxRect.top, boxRect.getHeight()) / boxRect.getHeight(), 0.f);
		
		colorPicker.isEditingSV = true;
	}

	// hue
	if (colorPicker.isEditingHue || (justClicked[0] && hueBar.contains(cursorPos))) {
		colorPicker.hueMod = std::max(std::min(cursorPos.x - hueBar.left, hueBar.getWidth()) / hueBar.getWidth(), 0.f);
		colorPicker.isEditingHue = true;
	}

	// alpha
	if (colorPicker.isEditingOpacity || (justClicked[0] && alphaBar.contains(cursorPos))) {
		float val = (colorPicker.opacityMod / (cPickerRect.getWidth()));

		float interval = 0.05f;

		// Find a good value to set to ("latch to nearest")
		val /= interval;
		val = std::round(val);
		val *= interval;

		colorPicker.opacityMod = val;
		
		colorPicker.opacityMod = std::max(std::min(cursorPos.x - alphaBar.left, alphaBar.getWidth()) / alphaBar.getWidth(), 0.f);
		colorPicker.isEditingOpacity = true;
	}

	if (!mouseButtons[0]) {
		colorPicker.isEditingSV = false;
		colorPicker.isEditingHue = false;
		colorPicker.isEditingOpacity = false;
	}

	{
		colorPicker.pickerColor.h = (colorPicker.hueMod * 360.f);
		colorPicker.pickerColor.s = colorPicker.svModX;
		colorPicker.pickerColor.v = 1.f - colorPicker.svModY;
	}


	// SV
	{
		auto ellipse = D2D1::Ellipse({ boxRect.left + (hueBar.getWidth() * colorPicker.svModX), boxRect.top + (boxRect.getHeight() * colorPicker.svModY)}, ellipseRadius, ellipseRadius);
		dc.brush->SetColor(col.asAlpha(1.f).get());
		dc.ctx->FillEllipse(ellipse, dc.brush);
		dc.brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		dc.ctx->DrawEllipse(ellipse, dc.brush, ellipseRadius / 2.f);
	}

	// hue
	{
		auto ellipse = D2D1::Ellipse({ hueBar.left + (hueBar.getWidth() * colorPicker.hueMod), hueBar.centerY() }, ellipseRadius, ellipseRadius);
		auto huedCol = util::HSVToColor({ colorPicker.hueMod * 360.f, 1.f, 1.f });
		dc.brush->SetColor(d2d::Color(huedCol).get());
		dc.ctx->FillEllipse(ellipse, dc.brush);
		dc.brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		dc.ctx->DrawEllipse(ellipse, dc.brush, ellipseRadius / 2.f);
	}

	// alpha
	{
		auto ellipse = D2D1::Ellipse({ alphaBar.left + (alphaBar.getWidth() * colorPicker.opacityMod), alphaBar.centerY() }, ellipseRadius, ellipseRadius);
		dc.brush->SetColor(col.asAlpha(colorPicker.opacityMod).get());
		dc.ctx->FillEllipse(ellipse, dc.brush);
		dc.brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		dc.ctx->DrawEllipse(ellipse, dc.brush, ellipseRadius / 2.f);
	}

	
	cPickerRect.bottom = alphaBar.bottom + remPad * 2.f;

	dc.ctx->SetTarget(Latite::getRenderer().getBitmap());

	// draw menu

	dc.fillRoundedRectangle(cPickerRect, d2d::Color::RGB(0x7, 0x7, 0x7).asAlpha(0.8f), 19.f * adaptedScale);
	dc.drawRoundedRectangle(cPickerRect, d2d::Color::RGB(0, 0, 0).asAlpha(0.28f), 19.f * adaptedScale, 4.f * adaptedScale, DXContext::OutlinePosition::Outside);
	
	// x button
	float xWidth = 0.06f * rectWidth;
	RectF xRect = { cPickerRect.right - xWidth * 2.f, cPickerRect.top + xWidth, cPickerRect.right - xWidth, cPickerRect.top + xWidth * 2.f };
	dc.ctx->DrawBitmap(Latite::getAssets().xIcon.getBitmap(), xRect);

	if (justClicked[0] && xRect.contains(cursorPos)) {
		colorPicker.queueClose = true;
		playClickSound();
	}

	// inner contents
	dc.ctx->DrawBitmap(shadowBitmap.Get());

	RectF pickerTopBar = { cPickerRect.left, cPickerRect.top, cPickerRect.right, boxRect.top };

	if (!colorPicker.dragging && justClicked[0] && pickerTopBar.contains(cursorPos)) {
		colorPicker.dragging = true;
		colorPicker.dragOffs = cursorPos - cPickerRect.getPos();
	}

	if (!mouseButtons[0]) colorPicker.dragging = false;

	if (colorPicker.dragging) {
		cPickerRect.setPos(cursorPos - colorPicker.dragOffs);
	}

	auto ss = Latite::getRenderer().getScreenSize();
	util::KeepInBounds(cPickerRect, { 0.f, 0.f, ss.width, ss.height });
}

void ClickGUI::onEnable(bool ignoreAnims) {
	calcAnim = 0.f;
	if (ignoreAnims) calcAnim = 1.f;
	this->tab = MODULES;
}

void ClickGUI::onDisable() {
	capturedKey = 0;
	activeSetting = nullptr;
	searchTextBox.reset();
	searchTextBox.setSelected(false);
	Latite::getConfigManager().saveCurrentConfig();
}