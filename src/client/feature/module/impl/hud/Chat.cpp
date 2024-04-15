#include "pch.h"
#include "Chat.h"

Chat::Chat() : HUDModule("Chat", "Chat", "A custom chat, replacing the vanilla chat.", HUD) {
	listen<ClientTextEvent>((EventListenerFunc)&Chat::onText);
	
	anchorData.addEntry(EnumEntry{ anchor_auto, "Auto" });
	anchorData.addEntry(EnumEntry{ anchor_top, "Top" });
	anchorData.addEntry(EnumEntry{ anchor_bottom, "Bottom" });

	setEnabled(true);
	//std::get<SnapValue>(snappingX).doSnapping = true;
	//std::get<SnapValue>(snappingX).type = SnapValue::Type::Normal;
	//std::get<SnapValue>(snappingX).position = SnapValue::Left;
	//std::get<SnapValue>(snappingX).index = 0;
	//
	//std::get<SnapValue>(snappingY).doSnapping = true;
	//std::get<SnapValue>(snappingY).type = SnapValue::Type::Normal;
	//std::get<SnapValue>(snappingX).position = SnapValue::Left;
	//std::get<SnapValue>(snappingY).index = 2;

	addSliderSetting("chatWidth", "Chat Width", "", chatWidth, FloatValue(100.f), FloatValue(1000.f), FloatValue(10.f));
	addSetting("backgroundColor", "Background Color", "The color of the background", backgroundColor);
	addSetting("textColor", "Text Color", "The color of the text", textColor);
	addEnumSetting("anchor", "Anchor", "How the chat window will be anchored", anchorData);
}

void Chat::render(DrawUtil& ctx, bool isDefault, bool inEditor) {
	d2d::Color bgCol = std::get<ColorValue>(backgroundColor).color1;
	d2d::Color fgCol = std::get<ColorValue>(textColor).color1;

	float windowWidth = std::get<FloatValue>(chatWidth);
	float windowHeight = messageHeight * maxMessages;
	auto tm = std::chrono::system_clock::now();

	for (auto& msg : messages) {
		if (tm - msg.timeCreated >= 5s) {
			messages.pop_back();
		}
	}


	for (size_t i = 0; i < std::min(messages.size(), static_cast<size_t>(maxMessages)); i++) {
		auto& msg = messages[i];

		auto sineCurve = [](float x) {
			return sin(x * 0.5f * pi_f);
		};

		if (tm - msg.timeCreated > 4s) {
			msg.animation = sineCurve(std::min(1.f, (1000.f / (float)std::chrono::duration_cast<std::chrono::milliseconds>(tm - msg.timeCreated - 4s).count()) - 1.f));
		}
		else if (tm - msg.timeCreated < 1s) {
			msg.animation = sineCurve((float)std::chrono::duration_cast<std::chrono::milliseconds>(tm - msg.timeCreated).count() / 1000.f);
		}
		
		d2d::Rect tRect = { 0, rect.getHeight() - messageHeight * (i + 1),
			rect.getWidth(), rect.getHeight() - messageHeight * i};

		ctx.fillRectangle(tRect, bgCol.asAlpha(msg.animation * bgCol.a));

		tRect = { (1.f - msg.animation) * -rect.getWidth(), rect.getHeight() - messageHeight * (i + 1),
			rect.getWidth() * (msg.animation), rect.getHeight() - messageHeight * i };

		/*we will cache the message in d2d. it's more worth it*/
		ctx.drawText(tRect, util::StrToWStr(msg.content), fgCol.asAlpha(msg.animation * fgCol.a), Renderer::FontSelection::SegoeRegular, textSize,
			DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, true);
	}

	rect.right = rect.left + windowWidth;
	rect.bottom = rect.top + windowHeight;
}

void Chat::onText(Event& evG) {
	auto& ev = reinterpret_cast<ClientTextEvent&>(evG);
	auto pkt = ev.getTextPacket();

	std::string content = pkt->str;
	if (pkt->type == SDK::TextPacketType::CHAT) {
		content = "<" + pkt->source.str() + "> " + content;
	}

	this->messages.emplace(messages.begin(), content);
}
