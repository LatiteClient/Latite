#include "HUDEditor.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/Eventing.h"
#include "client/Latite.h"
#include "util/DxUtil.h"
#include "util/DxContext.h"
#include "util/Util.h"
#include "../ScreenManager.h"

#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"

HUDEditor::HUDEditor() : Screen("HUDEditor") {
	Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&HUDEditor::onRender, 1);
}

void HUDEditor::onRender(Event& ev) {
	DXContext dc;
	float toBlur = Latite::get().getMenuBlur().value_or(0.f);

	auto alpha = sdk::ClientInstance::get()->minecraft->timer->alpha / 10.f;
	anim = std::lerp(anim, 1.f, alpha);

	if (Latite::get().getMenuBlur()) dc.drawGaussianBlur(toBlur * anim);

	auto& cursorPos = sdk::ClientInstance::get()->cursorPos;

	// Center Button
	{
		float buttonWidth = 200.f;
		float buttonHeight = 60.f;

		auto ss = dc.ctx->GetSize();

		d2d::Rect ssRec = { 0.f, 0.f, ss.width, ss.height };
		Vec2 btnPos = ssRec.center({ 200.f, 100.f });
		d2d::Rect btnRect = { btnPos, Vec2(btnPos.x + buttonWidth, btnPos.y + buttonHeight) };

		d2d::Color oCol = d2d::Color::RGB(0x7, 0x7, 0x7).asAlpha(0.75f);
		static d2d::Color col = oCol;
		d2d::Color outlineCol = d2d::Color::RGB(0, 0, 0).asAlpha(0.28f);

		bool state = shouldSelect(btnRect, cursorPos);
		if (state && mouseButtons[0]) {
			Latite::getScreenManager().exitCurrentScreen();
			Latite::getScreenManager().showScreen("ClickGUI");
			playClickSound();
		}

		col = util::LerpColorState(col, oCol + 0.1f, oCol, state);
		dc.fillRoundedRectangle(btnRect, col, 20.f);
		dc.drawRoundedRectangle(btnRect, outlineCol, 20.f, 2.f, DXContext::OutlinePosition::Outside);

		dc.setFont(Renderer::FontSelection::Regular);
		dc.drawText(btnRect, L"Mod Settings", d2d::Color(0.9f, 0.9f, 0.9f, 1.f), 20.f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
}

void HUDEditor::onEnable(bool ignoreAnims) {
	if (!ignoreAnims) anim = 0.f;
	sdk::ClientInstance::get()->releaseCursor();
}

void HUDEditor::onDisable() {
	sdk::ClientInstance::get()->grabCursor();
}
