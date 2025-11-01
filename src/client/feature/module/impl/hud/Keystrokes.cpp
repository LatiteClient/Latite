#include "pch.h"
#include "Keystrokes.h"
#include "client/input/Keyboard.h"
#include <array>
#include <client/Latite.h>

Keystrokes::Keystrokes() : HUDModule("Keystrokes", LocalizeString::get("client.hudmodule.keystrokes.name"),
                                     LocalizeString::get("client.hudmodule.keystrokes.desc"), HUD) {
    addSetting("mouseButtons", LocalizeString::get("client.hudmodule.keystrokes.mouseButtons.name"),
               LocalizeString::get("client.hudmodule.keystrokes.mouseButtons.desc"), mouseButtons);
    addSetting("showCps", LocalizeString::get("client.hudmodule.keystrokes.showCps.name"),
               LocalizeString::get("client.hudmodule.keystrokes.showCps.desc"), cps);
    addSetting("spaceBar", LocalizeString::get("client.hudmodule.keystrokes.spaceBar.name"),
               LocalizeString::get("client.hudmodule.keystrokes.spaceBar.desc"), spaceBar);
    addSetting("border", LocalizeString::get("client.hudmodule.keystrokes.border.name"),
               LocalizeString::get("client.hudmodule.keystrokes.border.desc"), border);
    addSetting("showSneak", LocalizeString::get("client.hudmodule.keystrokes.showSneak.name"),
               LocalizeString::get("client.hudmodule.keystrokes.showSneak.desc"), shiftKey);

    addSliderSetting("radius", LocalizeString::get("client.hudmodule.keystrokes.radius.name"),
                     LocalizeString::get("client.hudmodule.keystrokes.radius.desc"), radius, FloatValue(0.f),
                     FloatValue(10.f), FloatValue(1.f));
    addSliderSetting("textSize", LocalizeString::get("client.hudmodule.keystrokes.textSize.name"),
                     LocalizeString::get("client.hudmodule.keystrokes.textSize.desc"), textSize, FloatValue(2.f),
                     FloatValue(40.f), FloatValue(0.2f));
    addSliderSetting("keySize", LocalizeString::get("client.hudmodule.keystrokes.keySize.name"),
                     LocalizeString::get("client.hudmodule.keystrokes.keySize.desc"), keystrokeSize, FloatValue(15.f),
                     FloatValue(90.f), FloatValue(2.f));
    addSliderSetting("spaceSize", LocalizeString::get("client.hudmodule.keystrokes.spaceSize.name"),
                     LocalizeString::get("client.hudmodule.keystrokes.spaceSize.desc"), spaceSize, FloatValue(0.f),
                     FloatValue(90.f), FloatValue(2.f));
    addSliderSetting("mouseButtonHeight", LocalizeString::get("client.hudmodule.keystrokes.mouseButtonHeight.name"),
                     LocalizeString::get("client.hudmodule.keystrokes.mouseButtonHeight.desc"), mouseButtonHeight,
                     FloatValue(15.f), FloatValue(90.f), FloatValue(2.f), "mouseButtons"_istrue);
    addSliderSetting("padding", LocalizeString::get("client.hudmodule.keystrokes.padding.name"),
                     LocalizeString::get("client.hudmodule.keystrokes.padding.desc"), padding, FloatValue(0.f),
                     FloatValue(6.f), FloatValue(0.25f));
    addSliderSetting("borderLength", LocalizeString::get("client.hudmodule.keystrokes.borderLength.name"),
                     LocalizeString::get("client.hudmodule.keystrokes.borderLength.desc"), borderLength, FloatValue(0.f),
                     FloatValue(6.f), FloatValue(0.25f), "border"_istrue);
    addSliderSetting("transition", LocalizeString::get("client.hudmodule.keystrokes.transition.name"),
                     LocalizeString::get("client.hudmodule.keystrokes.transition.desc"), lerpSpeed, FloatValue(0.f),
                     FloatValue(3.f), FloatValue(0.05f));

    addSetting("borderCol", LocalizeString::get("client.hudmodule.keystrokes.borderCol.name"),
               LocalizeString::get("client.hudmodule.keystrokes.borderCol.desc"), borderColor);
    addSetting("pressedCol", LocalizeString::get("client.hudmodule.keystrokes.pressedCol.name"),
               LocalizeString::get("client.hudmodule.keystrokes.pressedCol.desc"), pressedColor);
    addSetting("unpressedCol", LocalizeString::get("client.hudmodule.keystrokes.unpressedCol.name"),
               LocalizeString::get("client.hudmodule.keystrokes.unpressedCol.desc"), unpressedColor);
    addSetting("ptCol", LocalizeString::get("client.hudmodule.keystrokes.ptCol.name"),
               LocalizeString::get("client.hudmodule.keystrokes.ptCol.desc"), pressedTextColor);
    addSetting("uptCol", LocalizeString::get("client.hudmodule.keystrokes.uptCol.name"),
               LocalizeString::get("client.hudmodule.keystrokes.uptCol.desc"), unpressedTextColor);

    listen<ClickEvent>((EventListenerFunc)&Keystrokes::onClick);
}


Vec2 Keystrokes::drawKeystroke(DrawUtil& ctx, Vec2 const& pos, Keystroke& stroke) {
	d2d::Rect front = { pos.x, pos.y, pos.x + std::get<FloatValue>(keystrokeSize), pos.y + std::get<FloatValue>(keystrokeSize) };
	float scale = std::get<FloatValue>(textSize);//ctx.scaleTextInBounds(key, 1 * textSize, (front.right - front.left), 2);
	ctx.fillRoundedRectangle(front, stroke.col, std::min(std::get<FloatValue>(this->radius).value, std::get<FloatValue>(keystrokeSize) / 2.f));
	if (std::get<BoolValue>(border)) ctx.drawRoundedRectangle(front, std::get<ColorValue>(borderColor).getMainColor(), std::get<FloatValue>(this->radius), std::get<FloatValue>(borderLength));
	ctx.drawText(front, stroke.keyName, stroke.textCol, Renderer::FontSelection::SecondaryLight, std::get<FloatValue>(textSize), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	return { front.right - front.left, front.bottom - front.top };
}

void Keystrokes::onClick(Event& evG) {
	auto& ev = reinterpret_cast<ClickEvent&>(evG);

	if (ev.getMouseButton() == 1) {
		primaryClickState = ev.isDown();
	}
	else if (ev.getMouseButton() == 2) {
		secondaryClickState = ev.isDown();
	}
}

void Keystrokes::render(DrawUtil& dc, bool, bool inEditor) {
	// W, S, A, D keys
	// + sneak, space, LMB, RMB

	// I fucking hate this, but Mojang decided to turn the input states into bits...
#define CREATE_GET(field) [&] -> bool { return field; }

	static std::array<Stroke, 2> mouseButtons = { Stroke(CREATE_GET(primaryClickState)), Stroke(CREATE_GET(secondaryClickState)) };

	static std::array<Keystroke, 6> keystrokes = {
		Keystroke("forward", CREATE_GET(SDK::ClientInstance::get()->getLocalPlayer()->getMoveInputComponent()->rawInputState.up)),
		Keystroke("left", CREATE_GET(SDK::ClientInstance::get()->getLocalPlayer()->getMoveInputComponent()->rawInputState.left)),
		Keystroke("back", CREATE_GET(SDK::ClientInstance::get()->getLocalPlayer()->getMoveInputComponent()->rawInputState.down)),
		Keystroke("right", CREATE_GET(SDK::ClientInstance::get()->getLocalPlayer()->getMoveInputComponent()->rawInputState.right)),
		Keystroke("sneak", CREATE_GET(SDK::ClientInstance::get()->getLocalPlayer()->getMoveInputComponent()->rawInputState.sneakDown)),
		Keystroke("jump", CREATE_GET(SDK::ClientInstance::get()->getLocalPlayer()->getMoveInputComponent()->rawInputState.jumpDown))
	};

#undef CREATE_GET
	

	float ls = std::get<FloatValue>(lerpSpeed);
	float lerpT = SDK::ClientInstance::get()->minecraft->timer->alpha * ls;

	for (auto& key : keystrokes) {
		if (ls > 0.01f) {
			key.col = util::LerpColorState(key.col, d2d::Color(std::get<ColorValue>(this->pressedColor).getMainColor()), d2d::Color(std::get<ColorValue>(this->unpressedColor).getMainColor()), key.get(), lerpT);
			key.textCol = util::LerpColorState(key.textCol, d2d::Color(std::get<ColorValue>(this->pressedTextColor).getMainColor()), d2d::Color(std::get<ColorValue>(this->unpressedTextColor).getMainColor()), key.get(), lerpT);
		}
		else {
			key.col = key.get() ? d2d::Color(std::get<ColorValue>(this->pressedColor).getMainColor()) : d2d::Color(std::get<ColorValue>(this->unpressedColor).getMainColor());
			key.textCol = key.get() ? d2d::Color(std::get<ColorValue>(this->pressedTextColor).getMainColor()) : d2d::Color(std::get<ColorValue>(this->unpressedTextColor).getMainColor());
		}
	
	}

	for (auto& btn : mouseButtons) {
		if (ls > 0.01f) {
			btn.col = util::LerpColorState(btn.col, d2d::Color(std::get<ColorValue>(this->pressedColor).getMainColor()), d2d::Color(std::get<ColorValue>(this->unpressedColor).getMainColor()), btn.get(), lerpT);
			btn.textCol = util::LerpColorState(btn.textCol, d2d::Color(std::get<ColorValue>(this->pressedTextColor).getMainColor()), d2d::Color(std::get<ColorValue>(this->unpressedTextColor).getMainColor()), btn.get(), lerpT);
		}
		else {
			btn.col = btn.get() ? d2d::Color(std::get<ColorValue>(this->pressedColor).getMainColor()) : d2d::Color(std::get<ColorValue>(this->unpressedColor).getMainColor());
			btn.textCol = btn.get() ? d2d::Color(std::get<ColorValue>(this->pressedTextColor).getMainColor()) : d2d::Color(std::get<ColorValue>(this->unpressedTextColor).getMainColor());
		}
	}

	// Direction Keys

	FloatValue pad = std::get<FloatValue>(padding);
	Vec2 pos = Vec2(std::get<FloatValue>(keystrokeSize) + pad, 0.f);
	pos.y += drawKeystroke(dc, pos, keystrokes[0]).y + pad; // w
	pos.x -= (drawKeystroke(dc, pos, keystrokes[2]).x + pad); // s
	pos.x += (drawKeystroke(dc, pos, keystrokes[1]).x + pad) * 2.f; // a
	pos = pos + (drawKeystroke(dc, pos, keystrokes[3]));

	// Mouse Buttons

	if (std::get<BoolValue>(this->mouseButtons)) {
		pos.y += pad;
		float mbHeight = std::get<FloatValue>(mouseButtonHeight);
		float rad = std::min(std::get<FloatValue>(this->radius).value, mbHeight / 2.f);
		{
			auto& btn = mouseButtons[0];
			d2d::Rect mb = { 0.f, pos.y, pos.x, pos.y + mbHeight };
			mb.right -= (mb.getWidth() / 2);
			dc.fillRoundedRectangle(mb, btn.col, rad);

			std::wstring str = L"LMB";

			if (std::get<BoolValue>(cps)) {
				str += L"\n" + std::to_wstring(Latite::get().getTimings().getCPSL()) + L" CPS";
			}

			dc.drawText(mb, str, btn.textCol, Renderer::FontSelection::SecondaryLight, std::get<FloatValue>(textSize), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			if (std::get<BoolValue>(border)) {
				dc.drawRoundedRectangle(mb, std::get<ColorValue>(this->borderColor).getMainColor(), rad, std::get<FloatValue>(this->borderLength));
			}
		}
		{
			auto& btn = mouseButtons[1];
			d2d::Rect mb = { 0.f, pos.y, pos.x, pos.y + mbHeight };
			mb.left += (mb.getWidth() / 2) + pad;

			dc.fillRoundedRectangle(mb, btn.col, rad);
			std::wstring str = L"RMB";

			if (std::get<BoolValue>(cps)) {
				str += L"\n" + std::to_wstring(Latite::get().getTimings().getCPSR()) + L" CPS";
			}

			dc.drawText(mb, str, btn.textCol, Renderer::FontSelection::SecondaryLight, std::get<FloatValue>(textSize), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			if (std::get<BoolValue>(border)) {
				dc.drawRoundedRectangle(mb, std::get<ColorValue>(this->borderColor).getMainColor(), rad, std::get<FloatValue>(this->borderLength));
			}
		}
		pos.y += mbHeight;
	}

	float rad = std::get<FloatValue>(radius);
	if (std::get<BoolValue>(spaceBar)) {
		pos.y += pad;
		d2d::Rect spaceBox = { 0.f, pos.y, pos.x, pos.y + std::get<FloatValue>(spaceSize) };
		dc.fillRoundedRectangle(spaceBox, keystrokes[5].col, rad);
		dc.flush(false);

		if (std::get<BoolValue>(border)) dc.drawRoundedRectangle(spaceBox, std::get<ColorValue>(borderColor).getMainColor(), rad, std::get<FloatValue>(borderLength));
		Vec2 center = spaceBox.center({ 1.f * std::get<FloatValue>(keystrokeSize), 1 });
		dc.fillRectangle({ center.x, center.y, center.x + (1.f * std::get<FloatValue>(keystrokeSize)), center.y + 1 }, keystrokes[5].textCol);
		pos.y += spaceBox.getHeight();
		dc.flush(false);
	}

	if (std::get<BoolValue>(shiftKey)) {
		pos.y += pad;
		d2d::Rect shiftBox = { 0.f, pos.y, pos.x, pos.y + std::get<FloatValue>(spaceSize) };
		dc.fillRoundedRectangle(shiftBox, keystrokes[4].col, rad);
		if (std::get<BoolValue>(border)) dc.drawRoundedRectangle(shiftBox, std::get<ColorValue>(borderColor).getMainColor(), rad, std::get<FloatValue>(borderLength));
		dc.drawText(shiftBox, keystrokes[4].keyName, keystrokes[4].textCol, Renderer::FontSelection::SecondaryLight, std::get<FloatValue>(textSize), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pos.y += shiftBox.getHeight();
	}

	int cpsL = 0;
	int cpsR = 0;

	if (!inEditor) {
		cpsL = Latite::get().getTimings().getCPSL();
		cpsR = Latite::get().getTimings().getCPSR();
	}

	this->rect.right = rect.left + pos.x;
	this->rect.bottom = rect.top + pos.y;
};

Keystrokes::Keystroke::Keystroke(std::string const& inputMapping, GetInputFunc getInput) : Stroke(getInput)
	, mapping(inputMapping) {
	vKey = Latite::getKeyboard().getMappedKey(inputMapping);
	keyName = util::StrToWStr(util::KeyToString(vKey));
}

void Keystrokes::Keystroke::updateKeyName() {
	vKey = Latite::getKeyboard().getMappedKey(mapping);
	keyName = util::StrToWStr(util::KeyToString(vKey));
}
