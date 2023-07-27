#include "ClickGUI.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RendererCleanupEvent.h"
#include "client/event/impl/RendererInitEvent.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/ClickEvent.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"
#include "client/feature/module/Module.h"
#include "client/feature/module/ModuleManager.h"
#include "util/Util.h"
#include "util/DxContext.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"
#include "client/render/Assets.h"

#include "../ScreenManager.h"

using FontSelection = Renderer::FontSelection;
using RectF = d2d::Rect;

namespace {
	static constexpr float setting_height_relative = 0.0168f;
}

ClickGUI::ClickGUI() : Screen("ClickGUI") {
	Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&ClickGUI::onRender, 1);
	Eventing::get().listen<RendererCleanupEvent>(this, (EventListenerFunc)&ClickGUI::onCleanup, 1, true);
	Eventing::get().listen<RendererInitEvent>(this, (EventListenerFunc)&ClickGUI::onInit, 1, true);
	Eventing::get().listen<KeyUpdateEvent>(this, (EventListenerFunc)&ClickGUI::onKey, 1);
	Eventing::get().listen<ClickEvent>(this, (EventListenerFunc)&ClickGUI::onClick, 1);
}

void ClickGUI::onRender(Event&) {
	bool shouldArrow = true;

	DXContext dc;
	sdk::ClientInstance::get()->releaseCursor();
	dc.ctx->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	Vec2& cursorPos = sdk::ClientInstance::get()->cursorPos;

	RECT desktopSize;
	HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktopSize);

	//auto& ev = reinterpret_cast<RenderOverlayEvent&>(evGeneric);
	auto& rend = Latite::getRenderer();
	auto ss = rend.getDeviceContext()->GetSize();

	float adaptedScale = ss.width / 1920.f;

	float guiX = ss.width / 4.f;
	float guiY = ss.height / 4.f;
	{
		float totalWidth = ss.height * (16.f / 9.f);;

		float realGuiX = totalWidth / 2.f;

		guiX = (ss.width / 2.f) - (realGuiX / 2.f);
		guiY = (ss.height / 4.f);
	}

	rect = { guiX, guiY, ss.width - guiX, ss.height - guiY };
	float guiWidth = rect.getWidth();

	d2d::Color outline = d2d::Color::RGB(0, 0, 0);
	outline.a = 0.28f;
	d2d::Color rcColor = d2d::Color::RGB(0x7, 0x7, 0x7);
	rcColor.a = 0.75f;
	rect.round();

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
	if (Latite::get().getMenuBlur()) dc.drawGaussianBlur(Latite::get().getMenuBlur().value());
	dc.fillRoundedRectangle(rect, rcColor, 19.f * adaptedScale);
	dc.drawRoundedRectangle(rect, outline, 19.f * adaptedScale, 4.f * adaptedScale, DXContext::OutlinePosition::Outside);
	dc.setFont(FontSelection::Semilight);
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
			auto sz = bmp->GetSize();

			D2D1::Matrix3x2F m;

			//dc.ctx->GetTransform(&m);
			//dc.ctx->SetTransform(D2D1::Matrix3x2F::Scale(41.f / sz.width, 41.f / sz.height) * D2D1::Matrix3x2F::Translation(logoRect.left, logoRect.top) * m);
			dc.ctx->DrawBitmap(bmp, logoRect, 1.f);
			//dc.ctx->DrawImage(compositeEffect.Get(), { 0.f, 0.f });
			//dc.ctx->SetTransform(m);
		}


		// Latite Text
		dc.setFont(FontSelection::Light);
		dc.drawText({ logoRect.right + 9.f * adaptedScale, logoRect.top + 3.f * adaptedScale, logoRect.right + 500.f, logoRect.top + 50.f * adaptedScale }, L"Latite Client", d2d::Color(1.f, 1.f, 1.f, 1.f), 25.f * adaptedScale, DWRITE_TEXT_ALIGNMENT_LEADING);
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
						// TODO: hud screen
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
		dc.setFont(FontSelection::Regular);
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

		{

			dc.ctx->SetTarget(shadowBitmap.Get());
			dc.ctx->Clear();

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
				if (this->tab == SETTINGS) {
					searchStr = L" Settings";
				}
				dc.drawText({ searchRect.left + 5.f + searchRect.getHeight(), searchRect.top, searchRect.right - 5.f + searchRect.getHeight(), searchRect.bottom }, L"Search" + searchStr, d2d::Color::RGB(0xB9, 0xB9, 0xB9), searchRect.getHeight() / 2.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				dc.ctx->DrawBitmap(Latite::getAssets().searchIcon.getBitmap(), { searchRect.left + 10.f, searchRect.top + 6.f, searchRect.left - 3.f + searchRect.getHeight(), searchRect.top + searchRect.getHeight() - 6.f });
			}

			dc.ctx->SetTarget(myBitmap);
		}

		if (tab == MODULES) {

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

				dc.drawText(nodeRect, std::get<0>(pair), {1.f, 1.f, 1.f, 0.8f}, nodeRect.getHeight() / 2.f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				dc.ctx->SetTarget(myBitmap);

				auto oWidth = nodeRect.getWidth() + gaps;
				nodeRect.left += oWidth;
				nodeRect.right += oWidth;

			}
		}
		dc.ctx->DrawImage(compositeEffect.Get());
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
			if (modTab == HUD && mod.mod->getCategory() == IModule::GAME) mod.shouldRender = false; // Hud Tab
			if (modTab == SCRIPT && mod.mod->getCategory() != IModule::SCRIPT) mod.shouldRender = false; // Hud Tab
		}

		int i = 0;
		int row = 1;
		int column = 1;
		for (auto& mod : mods) {
			if (!mod.shouldRender) continue;
			Vec2 pos = { x, y };
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

					dc.drawText(descTextRect, util::StrToWStr(mod.description), d2d::Color(1.f, 1.f, 1.f, 0.57f), textSizeDesc, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

					float padToSetting = 0.014184f * rect.getHeight();
					float settingPadY = padToSetting * 1.5f;
					float settingHeight = rect.getHeight() * setting_height_relative;

					modRectActual.bottom += padToSetting;
					mod.mod->settings->forEach([&](std::shared_ptr<Setting> set) {
						if (set->type == Setting::Type::Bool) {
							drawSetting(set.get(), { descTextRect.left, modRectActual.bottom }, modRect.getWidth() / 2.f);
							modRectActual.bottom += settingHeight;
							modRectActual.bottom += settingPadY;
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
			dc.setFont(FontSelection::Light);
			dc.drawText(textRect, util::StrToWStr(mod.name), { 1.f, 1.f, 1.f, 1.f }, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

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
				dc.ctx->SetTransform(D2D1::Matrix3x2F::Rotation(mod.lerpArrowRot, { rc.centerX(), rc.centerY() }));
				mod.lerpArrowRot = std::lerp(mod.lerpArrowRot, toLerp, sdk::ClientInstance::get()->minecraft->timer->alpha * 0.3f);
				// icon
				dc.ctx->DrawBitmap(Latite::getAssets().arrowIcon.getBitmap(), rc.get());
				dc.ctx->SetTransform(oMatr);
			}

			if (i >= numMods) {
				i = 0;
				row++;
				column = 0;
				y += modRectActual.getHeight() + padFromSearchBar;
				x = xStart;
			}
			else {
				x += modBetwPad + modWidth;
				column++;
			}
			i++;
		}

	}

	if (shouldArrow) cursor = Cursor::Arrow;
}

void ClickGUI::onCleanup(Event&) {
	this->compositeEffect = nullptr;
}

void ClickGUI::onInit(Event&) {
	auto myBitmap = Latite::getRenderer().getBitmap();
	D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
	D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();

	Latite::getRenderer().getDeviceContext()->CreateBitmap(bitmapSize, nullptr, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, pixelFormat), shadowBitmap.GetAddressOf());
	Latite::getRenderer().getDeviceContext()->CreateEffect(CLSID_D2D1Composite, compositeEffect.GetAddressOf());
}

void ClickGUI::onKey(Event& evGeneric) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
	ev.setCancelled(true);
}

void ClickGUI::onClick(Event& evGeneric) {
	auto& ev = reinterpret_cast<ClickEvent&>(evGeneric);
	if (ev.getMouseButton() > 0) {
		ev.setCancelled(true);
	}
}

void ClickGUI::drawSetting(Setting* set, Vec2 const& pos, float size) {
	DXContext dc;
	switch (set->type) {
	case Setting::Type::Bool:
	{
		float checkboxSize = rect.getWidth() * setting_height_relative;
		RectF checkboxRect = { pos.x, pos.y, pos.x + checkboxSize, pos.y + checkboxSize };
		bool contains = this->shouldSelect(checkboxRect, sdk::ClientInstance::get()->cursorPos);

		float round = 0.1875f * checkboxSize;
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
		auto dis2p = util::StrToWStr(set->name());

		dc.setFont(FontSelection::Regular);
		dc.drawText(textRect, disp, { 1.f, 1.f, 1.f, 1.f }, checkboxSize * 0.8f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		if (contains || shouldSelect(textRect, sdk::ClientInstance::get()->cursorPos)) tooltip = util::StrToWStr(set->desc());
	}
		break;
	default:
		break;
	}
}

void ClickGUI::onEnable(bool ignoreAnims) {
	this->tab = MODULES;
}

void ClickGUI::onDisable() {
}
