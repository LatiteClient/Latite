#pragma once
#include <client/feature/module/HUDModule.h>

class Chat : public HUDModule {
public:
	Chat();
	virtual ~Chat() {};

	void render(DrawUtil& ctx, bool isDefault, bool inEditor) override;
	virtual bool forceMinecraftRenderer() override { return true; }
private:
	void onText(Event&);
	void onLatiteMessage(Event&);
	void onRenderLayer(Event&);

	void addMessage(std::string const& message);

	struct ChatMessage {
		std::chrono::system_clock::time_point timeCreated;
		std::string content = "";
		int duplicate = 1;
		float animation = 0.f;

		ChatMessage(std::string content) : content(std::move(content)) {
			timeCreated = std::chrono::system_clock::now();
		}
	};

	std::vector<ChatMessage> messages;
	int maxMessages = 15;

	ValueType chatWidth = FloatValue(500.f);
	ValueType backgroundColor = ColorValue(0.f, 0.f, 0.f, 0.5f);
	ValueType textColor = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType antiSpam = BoolValue(true);

	static constexpr int anchor_auto = 0;
	static constexpr int anchor_top = 0;
	static constexpr int anchor_bottom = 1;
	EnumData anchorData;

	float textSize = 30.f;
	float messageHeight = textSize;
};