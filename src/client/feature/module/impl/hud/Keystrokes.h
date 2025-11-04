#pragma once
#include "../../HUDModule.h"

class Keystrokes : public HUDModule {
public:
	Keystrokes();

	void render(DrawUtil& dc, bool, bool) override;
private:
	ValueType mouseButtons = BoolValue(true);
	ValueType cps = BoolValue(false);
	ValueType spaceBar = BoolValue(true);
	ValueType border = BoolValue(false);
	ValueType shiftKey = BoolValue(false);

	ValueType textSize = FloatValue(23.f);
	ValueType keystrokeSize = FloatValue(60.f);
	ValueType spaceSize = FloatValue(30.f);
	ValueType mouseButtonHeight = FloatValue(30.f);
	ValueType padding = FloatValue(3.f);
	ValueType borderLength = FloatValue(2.f);
	ValueType lerpSpeed = FloatValue(1.f);
	ValueType radius = FloatValue(0.f);

	ValueType borderColor = ColorValue(1.f, 1.f, 1.f, 0.5f);
	ValueType pressedColor = ColorValue(1.f, 1.f, 1.f, 0.5f);
	ValueType unpressedColor = ColorValue(0.f, 0.f, 0.f, 0.5f);
	ValueType pressedTextColor = ColorValue(0.f, 0.f, 0.f, 1.f);
	ValueType unpressedTextColor = ColorValue(1.f, 1.f, 1.f, 1.f);

	typedef std::function<bool()> GetInputFunc;

	struct Stroke {
		d2d::Color col;
		d2d::Color textCol;
		std::wstring keyName;
		GetInputFunc getInput;

		Stroke(GetInputFunc getInput) : getInput(getInput) {
		}

		[[nodiscard]] bool get() const {
			return getInput();
		}
	};

	struct Keystroke : public Stroke {
		std::string mapping;
		int vKey;

		Keystroke(std::string const& inputMapping, GetInputFunc getInput);

		void updateKeyName();
	};

	Vec2 drawKeystroke(DrawUtil& ctx, Vec2 const& pos, Keystroke& stroke);

	void onClick(Event& evG);

	bool primaryClickState = false;
	bool secondaryClickState = false;
};