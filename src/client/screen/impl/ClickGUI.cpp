#include "ClickGUI.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RendererCleanupEvent.h"
#include "client/event/impl/RendererInitEvent.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/ClickEvent.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"
#include "client/feature/module/ModuleManager.h"
#include "util/Util.h"
#include "util/DxContext.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"
#include "client/render/Assets.h"

using FontSelection = Renderer::FontSelection;
using RectF = d2d::Rect;

ClickGUI::ClickGUI() : Screen("ClickGUI"), blurBuffer(nullptr) {
	Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&ClickGUI::onRender);
	Eventing::get().listen<RendererCleanupEvent>(this, (EventListenerFunc)&ClickGUI::onCleanup);
	Eventing::get().listen<RendererInitEvent>(this, (EventListenerFunc)&ClickGUI::onInit, 0, true);
	Eventing::get().listen<KeyUpdateEvent>(this, (EventListenerFunc)&ClickGUI::onKey);
	Eventing::get().listen<ClickEvent>(this, (EventListenerFunc)&ClickGUI::onClick);
}

void ClickGUI::onRender(Event&) {
	DXContext dc;
	sdk::ClientInstance::get()->releaseCursor();
	dc.ctx->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	for (size_t i = 0; i < justClicked.size(); i++) {
		justClicked[i] = this->lastMouseButtons[i] != this->mouseButtons[i] && this->mouseButtons[i];
	}

	RECT desktopSize;
	HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktopSize);

	//auto& ev = reinterpret_cast<RenderOverlayEvent&>(evGeneric);
	auto& rend = Latite::getRenderer();
	auto ss = rend.getDeviceContext()->GetSize();

	float adaptedScale = ss.width / 1920.f;

	float guiX = ss.width / 4.f;
	float guiY = ss.height / 4.f;

	// Calc Size
	/* {
		float width = static_cast<float>(desktopSize.right - desktopSize.left);
		float height = static_cast<float>(desktopSize.bottom - desktopSize.top);

		adaptedScale = width / 1920.f;

		float realGuiX = width / 4.f;
		float realGuiY = height / 4.f;

		float leewayX = (width / 2) - realGuiX;
		float leewayY = (height / 2) - realGuiY;

		guiX = ((ss.width / 2.f) - (leewayX));
		guiY = ((ss.height / 2.f) - (leewayY));
	}*/

	{
		float totalWidth = ss.height * (16.f / 9.f);;

		float realGuiX = totalWidth / 2.f;

		guiX = (ss.width / 2.f) - (realGuiX / 2.f);
		guiY = (ss.height / 4.f);
	}

	RectF rect = { guiX, guiY, ss.width - guiX, ss.height - guiY };
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

	D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Translation(10.f * adaptedScale, 10.f * adaptedScale);
	affineTransformEffect->SetInputEffect(0, shadowEffect);
	affineTransformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, mat);
	// Shadow effect bitmap
	auto myBitmap = rend.getBitmap();
	//

	// Menu Rect
	dc.drawGaussianBlur(20.f);
	dc.fillRoundedRectangle(rect, rcColor, 19.f * adaptedScale);
	dc.drawRoundedRectangle(rect, outline, 19.f * adaptedScale, 4.f * adaptedScale, DXContext::OutlinePosition::Outside);
	dc.setFont(FontSelection::Semilight);
	//dc.drawText({ 0, 0, 200, 30 }, "Hello world!", D2D1::ColorF::White, 30.f, DWRITE_TEXT_ALIGNMENT_CENTER);

	float offX = 0.01689 * rect.getWidth();
	float offY = 0.03191 * rect.getHeight();
	float imgSize = 0.05338 * rect.getWidth();

	D2D1_RECT_F logoRect = { rect.left + offX, rect.top + offY, rect.left + offX + imgSize, rect.top + offY + imgSize };

	// Latite Logo + text
	{
		{
			auto bmp = Latite::getAssets().latiteLogo.getBitmap();

			shadowEffect->SetInput(0, bmp);
			compositeEffect->SetInputEffect(0, affineTransformEffect);
			compositeEffect->SetInput(1, bmp);
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
		dc.drawText({ logoRect.right + 9.f * adaptedScale, logoRect.top + 3.f, logoRect.right + 500.f, logoRect.top + 50.f * adaptedScale }, "Latite Client", d2d::Color(1.f, 1.f, 1.f, 1.f), 25.f * adaptedScale, DWRITE_TEXT_ALIGNMENT_LEADING);
	}
	// Search Bar + tabs 
	RectF searchRect{};
	{
		dc.setFont(FontSelection::Regular);
		float gaps = guiWidth * 0.02f;

		// prototype height = 564

		float searchWidth = guiWidth * 0.25f;
		float searchHeight = 0.0425f * rect.getHeight();
		float searchRound = searchHeight * 0.416;

		searchRect = { logoRect.left, logoRect.bottom + gaps, logoRect.left + searchWidth, logoRect.bottom + gaps + searchHeight };
		auto searchCol = d2d::Color::RGB(0x70, 0x70, 0x70).asAlpha(0.28f);

		ComPtr<ID2D1Bitmap1> shadowBitmap;
		D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
		D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();

		dc.ctx->CreateBitmap(bitmapSize, nullptr, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, pixelFormat), shadowBitmap.GetAddressOf());

		{

			dc.ctx->SetTarget(shadowBitmap.Get());
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
				std::string searchStr = "";
				if (this->tab == SETTINGS) {
					searchStr = " Settings";
				}
				dc.drawText({ searchRect.left + 5.f + searchRect.getHeight(), searchRect.top, searchRect.right - 5.f + searchRect.getHeight(), searchRect.bottom }, "Search" + searchStr, d2d::Color::RGB(0xB9, 0xB9, 0xB9), searchRect.getHeight() / 2.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				dc.ctx->DrawBitmap(Latite::getAssets().searchIcon.getBitmap(), { searchRect.left + 10.f, searchRect.top + 6.f, searchRect.left - 3.f + searchRect.getHeight(), searchRect.top + searchRect.getHeight() - 6.f });
			}

			dc.ctx->SetTarget(myBitmap);
		}

		{
			// all, game, hud, etc buttons
			std::vector<std::pair<std::string, ClickGUI::ModTab>> modTabs = { {"All", ALL }, {"Game", GAME}, {"HUD", HUD}, {"Script", SCRIPT} };

			float nodeWidth = guiWidth * 0.083f;

			RectF nodeRect = { searchRect.right + gaps, searchRect.top, searchRect.right + gaps + nodeWidth, searchRect.bottom };


			for (auto& pair : modTabs) {
				dc.ctx->SetTarget(shadowBitmap.Get());
				D2D1_ROUNDED_RECT rr;
				rr.radiusX = searchRound;
				rr.radiusY = searchRound;
				rr.rect = nodeRect.get();
				auto solidBrush = rend.getSolidBrush();
				solidBrush->SetColor(searchCol.get());
				dc.ctx->FillRoundedRectangle(rr, rend.getSolidBrush());

				dc.drawText(nodeRect, pair.first, {1.f, 1.f, 1.f, 0.8f}, nodeRect.getHeight() / 2.f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
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

		for (auto& mod : mods) {
			mod.shouldRender = true;

			//if (modTab == GAME && mod.mod->getCategory() == ModCategory::Hud) mod.shouldRender = false; // Game Tab
			//if (modTab == HUD && mod.mod->getCategory() == ModCategory::Game) mod.shouldRender = false; // Hud Tab
		}

		int i = 0;
		int row = 1;
		int column = 1;
		for (auto& mod : mods) {
			if (!mod.shouldRender) continue;
			Vec2 pos = { x, y };
			RectF modRect = { pos.x, pos.y, pos.x + modWidth, pos.y + modHeight };

			dc.fillRoundedRectangle(modRect, d2d::Color::RGB(0x44, 0x44, 0x44).asAlpha(0.22f), .22f * modHeight);
			if (mod.mod->isEnabled()) dc.drawRoundedRectangle(modRect, d2d::Color::RGB(0x32, 0x39, 0x76).asAlpha(1.f), .22f * modHeight, 1.f, DXContext::OutlinePosition::Inside);;

			// arrow
			{
				RectF rc = { modRect.left + (modRect.getHeight() * 0.4f),
					modRect.top + (modRect.getHeight() * 0.4f), modRect.left + modRect.getHeight() * 0.70f, modRect.bottom - modRect.getHeight() * 0.4f };

				if (this->shouldSelect(rc, sdk::ClientInstance::get()->cursorPos)) {
					if (justClicked[0])
						mod.isExtended = !mod.isExtended;
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
			// text
			auto textRect = modRect;
			textRect.left += modRect.getWidth() / 6.f;
			dc.setFont(FontSelection::Light);
			float textHeight = 0.4f * modHeight;
			dc.drawText(textRect, mod.name, { 1.f, 1.f, 1.f, 1.f }, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

			// toggle
			{
				float togglePad = modHeight * 0.249f;
				float toggleWidth = modWidth * 0.143f;
				RectF toggleRect = { modRect.right - togglePad - toggleWidth, modRect.top + togglePad,
				modRect.right - togglePad, modRect.bottom - togglePad };

				if (this->shouldSelect(toggleRect, sdk::ClientInstance::get()->cursorPos)) {
					if (justClicked[0])
					mod.mod->setEnabled(!mod.mod->isEnabled());
				}

				//float aTogglePadY = toggleRect.getHeight() * 0.15f;
				float radius = toggleRect.getHeight() * 0.35f;
				float circleOffs = toggleWidth * 0.27f;

				dc.fillRoundedRectangle(toggleRect, mod.mod->isEnabled() ? d2d::Color::RGB(0x32, 0x39, 0x76) : d2d::Color::RGB(0x63, 0x63, 0x63), toggleRect.getHeight() / 2.f);
				Vec2 center{ toggleRect.left + circleOffs, toggleRect.centerY() };
				Vec2 center2 = center;
				center2.x = toggleRect.right - circleOffs;
				float onDist = center2.x - center.x;

				mod.lerpToggle = std::lerp(mod.lerpToggle, mod.mod->isEnabled() ? 1.f : 0.f, sdk::ClientInstance::get()->minecraft->timer->alpha * 0.3f);

				center.x += onDist * mod.lerpToggle;

				dc.brush->SetColor((d2d::Color(0xB9, 0xB9, 0xB9)).get());
				dc.ctx->FillEllipse(D2D1::Ellipse({ center.x, center.y }, radius, radius), dc.brush);
			}

			if (i >= numMods) {
				i = 0;
				row++;
				column = 0;
				y += modHeight + padFromSearchBar;
				x = xStart;
			}
			else {
				x += modBetwPad + modWidth;
				column++;
			}
			i++;
		}

	}

	this->lastMouseButtons = this->mouseButtons;
}

void ClickGUI::onCleanup(Event&) {
	this->blurBuffer = nullptr;
	this->compositeEffect = nullptr;
}

void ClickGUI::onInit(Event&) {
	Latite::getRenderer().getDeviceContext()->CreateEffect(CLSID_D2D1Composite, compositeEffect.GetAddressOf());
	this->blurBuffer = Latite::getRenderer().copyCurrentBitmap();
}

void ClickGUI::onKey(Event& evGeneric) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
	ev.setCancelled(true);
}

void ClickGUI::onClick(Event& evGeneric) {
	auto& ev = reinterpret_cast<ClickEvent&>(evGeneric);
	if (ev.getMouseButton() < 4) {
		this->mouseButtons[ev.getMouseButton() - 1] = ev.isDown();
	}
	if (ev.getMouseButton() > 0) {
		ev.setCancelled(true);
	}
}
