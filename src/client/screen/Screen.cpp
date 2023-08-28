#include "Screen.h"
#include "client/Latite.h"
#include "client/event/Eventing.h"
#include "ScreenManager.h"
#include "client/event/impl/ClickEvent.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/UpdateEvent.h"
#include "util/DxContext.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "pch.h"

Screen::Screen() {
	/*
	arrow = LoadCursorW(Latite::get().dllInst, IDC_ARROW);
	hand = LoadCursorW(Latite::get().dllInst, IDC_HAND);
	ibeam = LoadCursorW(Latite::get().dllInst, IDC_IBEAM);
	*/
	// ^ this doesnt work with resources...

	Eventing::get().listen<UpdateEvent>(this, (EventListenerFunc)&Screen::onUpdate, 0);
	Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&Screen::onRenderOverlay, 0, true);
	Eventing::get().listen<ClickEvent>(this, (EventListenerFunc)&Screen::onClick, 3, true);
}

void Screen::onUpdate(Event& ev) {
	/*
	switch (cursor) {
	case Cursor::Arrow:
		SetCursor(arrow);
		break;
	case Cursor::Hand:
		SetCursor(hand);
		break;
	case Cursor::IBeam:
		SetCursor(ibeam);
		break;
	}
	*/
	SDK::ClientInstance::get()->releaseCursor();
}

void Screen::close() {
	this->activeMouseButtons = { false, false, false };
	this->mouseButtons = { false, false, false };
	Latite::getScreenManager().exitCurrentScreen();
}

void Screen::playClickSound() {
	util::PlaySoundUI("random.click");
}

void Screen::onClick(Event& evGeneric) {
	auto& ev = reinterpret_cast<ClickEvent&>(evGeneric);
	if (ev.getMouseButton() > 0) {
		if (ev.getMouseButton() < 4) {
			if (isActive()) {
				if (ev.isDown())
					this->activeMouseButtons[ev.getMouseButton() - 1] = ev.isDown();
				this->mouseButtons[ev.getMouseButton() - 1] = ev.isDown();
			}
			if (isActive()) ev.setCancelled(true);
		}
	}
}

void Screen::onRenderOverlay(Event& ev) {
	if (this->isActive())
		for (size_t i = 0; i < justClicked.size(); i++) {
			justClicked[i] = this->activeMouseButtons[i];
			this->activeMouseButtons[i] = false;
		}

	if (isActive() && this->tooltip.has_value()) {
		DXContext dc;
		Vec2& mousePos = SDK::ClientInstance::get()->cursorPos;
		d2d::Rect textRect = dc.getTextRect(this->tooltip.value(), Renderer::FontSelection::SegoeRegular, 15.f, 8.f);
		textRect.setPos(mousePos);
		auto height = textRect.getHeight() * 0.9f;
		textRect.top -= height;
		textRect.bottom -= height;
		textRect.left += 5.f;
		textRect.right += 5.f;
		dc.fillRectangle(textRect, d2d::Color(0.f, 0.f, 0.f, 0.6f));
		dc.drawText(textRect, this->tooltip.value(), d2d::Color(1.f, 1.f, 1.f, 0.8f), Renderer::FontSelection::SegoeRegular, 15.f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	this->tooltip = std::nullopt;
}
