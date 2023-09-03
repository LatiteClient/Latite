#include "pch.h"
#include "Keystrokes.h"
#include "client/input/Keyboard.h"
#include <array>

Keystrokes::Keystrokes() : HUDModule("Keystrokes", "Keystrokes", "Shows movement keys") {
	addSetting("mouseButtons", "Mouse Buttons", "Show mouse buttons", mouseButtons);
	addSetting("showCps", "CPS", "Show clicks per second", cps);
	addSetting("spaceBar", "Space Bar", "Space bar", spaceBar);
	addSetting("border", "Border", "Border", border);
	addSetting("showSneak", "Sneak Key", "Shows the sneak key", shiftKey);

	addSliderSetting("radius", "Radius", "How round the buttons are", radius, FloatValue(0.f), FloatValue(10.f), FloatValue(1.f));
	addSliderSetting("textSize", "Text Size", "Text Size", textSize,   FloatValue(2.f), FloatValue(15.f), FloatValue(0.2f));
	addSliderSetting("keySize", "Key Size", "Key Size", keystrokeSize, FloatValue(15.f), FloatValue(90.f), FloatValue(2.f));
	addSliderSetting("spaceSize", "Space Bar", "Spacebar Size", spaceSize, FloatValue(0.f), FloatValue(90.f), FloatValue(2.f));
	addSliderSetting("mouseButtonHeight", "Mouse Button Height", "Mouse Button Height", mouseButtonHeight, FloatValue(15.f), FloatValue(90.f), FloatValue(2.f), "showCps"_istrue);
	addSliderSetting("padding", "Padding", "Padding between keys", padding, FloatValue(0.f), FloatValue(6.f), FloatValue(0.25f));
	addSliderSetting("borderLength", "Border Length", "The border length", borderLength, FloatValue(0.f), FloatValue(6.f), FloatValue(0.25f), "border"_istrue);
	addSliderSetting("transition", "Transition", "The smooth color transition", lerpSpeed, FloatValue(0.05f), FloatValue(1.f), FloatValue(0.05f));
	
	addSetting("borderCol", "Border Color", "The color of the border", borderColor);
	addSetting("pressedCol", "Pressed Color", "The key color when pressed", pressedColor);
	addSetting("unpressedCol", "Unpressed Color", "The key color when not pressed", unpressedColor);
	addSetting("ptCol", "Pressed Text Color", "The text color when pressed", pressedTextColor);
	addSetting("uptCol", "Unpressed Text Color", "The text color when not pressed", unpressedTextColor);
}

Vec2 Keystrokes::drawKeystroke(DXContext& ctx, Vec2 const& pos, Keystroke& stroke) {
	d2d::Rect front = { pos.x, pos.y, pos.x + std::get<FloatValue>(keystrokeSize), pos.y + std::get<FloatValue>(keystrokeSize) };
	float scale = std::get<FloatValue>(textSize);//ctx.scaleTextInBounds(key, 1 * textSize, (front.right - front.left), 2);
	ctx.fillRoundedRectangle(front, stroke.col, std::min(std::get<FloatValue>(this->radius).value, std::get<FloatValue>(keystrokeSize) / 2.f));
	if (std::get<BoolValue>(border)) ctx.drawRoundedRectangle(front, std::get<ColorValue>(borderColor).color1, std::get<FloatValue>(this->radius), std::get<FloatValue>(borderLength));
	ctx.drawText(front, stroke.keyName, stroke.textCol, Renderer::FontSelection::SegoeLight, std::get<FloatValue>(textSize), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	return { front.right - front.left, front.bottom - front.top };
}

void Keystrokes::render(DXContext& dc, bool, bool inEditor) {
	auto input = SDK::ClientInstance::get()->getLocalPlayer()->getMoveInputComponent();

	bool front = input->front;
	bool left = input->left;
	bool back = input->back;
	bool right = input->right;

	// W, S, A, D keys
	// + sneak, space, LMB, RMB

	static std::array<Stroke, 2> mouseButtons = { Stroke(ClickMap::get()->mouseButtons[1]), Stroke(ClickMap::get()->mouseButtons[2]) };

	static std::array<Keystroke, 6> keystrokes = { Keystroke("forward", input->front), Keystroke("left", input->left),
		Keystroke("back", input->back), Keystroke("right", input->right), Keystroke("sneak", input->sneak), Keystroke("jump", input->jump)};
	

	float lerpT = SDK::ClientInstance::get()->minecraft->timer->alpha * std::get<FloatValue>(lerpSpeed);

	for (auto& key : keystrokes) {
		key.updateKeyName();

		key.col = util::LerpColorState(key.col, d2d::Color(std::get<ColorValue>(this->pressedColor).color1), d2d::Color(std::get<ColorValue>(this->unpressedColor).color1), key.input.get(), lerpT);
		key.textCol = util::LerpColorState(key.textCol, d2d::Color(std::get<ColorValue>(this->pressedTextColor).color1), d2d::Color(std::get<ColorValue>(this->unpressedTextColor).color1), key.input.get(), lerpT);
	}

	for (auto& btn : mouseButtons) {
		btn.col = util::LerpColorState(btn.col, d2d::Color(std::get<ColorValue>(this->pressedColor).color1), d2d::Color(std::get<ColorValue>(this->unpressedColor).color1), btn.input.get(), lerpT);
		btn.textCol = util::LerpColorState(btn.textCol, d2d::Color(std::get<ColorValue>(this->pressedTextColor).color1), d2d::Color(std::get<ColorValue>(this->unpressedTextColor).color1), btn.input.get(), lerpT);
	}

	// Direction Keys

	FloatValue pad = std::get<FloatValue>(padding);
	Vec2 pos = Vec2(pad + std::get<FloatValue>(keystrokeSize) + pad, pad);
	pos.y += drawKeystroke(dc, pos, keystrokes[0]).y + pad; // w
	pos.x -= (drawKeystroke(dc, pos, keystrokes[2]).x + pad); // s
	pos.x += (drawKeystroke(dc, pos, keystrokes[1]).x + pad) * 2; // a
	pos = pos + (drawKeystroke(dc, pos, keystrokes[3]) + Vec2(0, pad)); // d

	// Mouse Buttons

	if (std::get<BoolValue>(this->mouseButtons)) {
		float mbHeight = std::get<FloatValue>(mouseButtonHeight);
		float rad = std::min(std::get<FloatValue>(this->radius).value, mbHeight / 2.f);
		{
			auto& btn = mouseButtons[0];
			d2d::Rect mb = { pad, pos.y, pos.x, pos.y + mbHeight };
			mb.right -= (mb.getWidth() / 2);
			dc.fillRoundedRectangle(mb, btn.col, std::get<FloatValue>(radius) * mb.getHeight());

			std::wstring str = L"LMB";
			dc.drawText(mb, str, btn.textCol, Renderer::FontSelection::SegoeLight, std::get<FloatValue>(textSize), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			if (std::get<BoolValue>(border)) {
				dc.drawRoundedRectangle(mb, std::get<ColorValue>(this->borderColor).color1, rad);
			}
		}
		{
			auto& btn = mouseButtons[1];
			d2d::Rect mb = { pad, pos.y, pos.x, pos.y + mbHeight };
			mb.left += (mb.getWidth() / 2) + pad;

			dc.fillRoundedRectangle(mb, btn.col, rad);
			std::wstring str = L"LMB";
			dc.drawText(mb, str, btn.textCol, Renderer::FontSelection::SegoeLight, std::get<FloatValue>(textSize), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			if (std::get<BoolValue>(border)) {
				dc.drawRoundedRectangle(mb, std::get<ColorValue>(this->borderColor).color1, rad);
			}
		}
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

Keystrokes::Keystroke::Keystroke(std::string const& inputMapping, bool& inputKey) : Stroke(inputKey)
	, mapping(inputMapping) {
	vKey = Latite::getKeyboard().getMappedKey(inputMapping);
	keyName = util::StrToWStr(util::KeyToString(vKey));
}

void Keystrokes::Keystroke::updateKeyName() {
	vKey = Latite::getKeyboard().getMappedKey(mapping);
	keyName = util::StrToWStr(util::KeyToString(vKey));
}
