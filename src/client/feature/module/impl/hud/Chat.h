#pragma once
#include <client/feature/module/HUDModule.h>

class Chat : public HUDModule {
public:
	Chat();
	virtual ~Chat() {};

	void render(DrawUtil& ctx, bool isDefault, bool inEditor) override;
	void onText(Event&);
private:
	struct ChatMessage {
		std::chrono::system_clock::time_point timeCreated;
		std::string content = "";
		float animation = 0.f;

		ChatMessage(std::string content) : content(std::move(content)) {
			timeCreated = std::chrono::system_clock::now();
		}
	};

	std::vector<ChatMessage> messages;
	int maxMessages = 10;

	ValueType chatWidth = FloatValue(500.f);
	ValueType backgroundColor = ColorValue(0.f, 0.f, 0.f, 0.5f);
	ValueType textColor = ColorValue(1.f, 1.f, 1.f, 1.f);

	static constexpr int anchor_auto = 0;
	static constexpr int anchor_top = 0;
	static constexpr int anchor_bottom = 1;
	EnumData anchorData;

	float textSize = 30.f;
	float messageHeight = textSize;
};