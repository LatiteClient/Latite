#include "pch.h"
#include "Chat.h"
#include <mc/common/client/gui/ScreenView.h>
#include <mc/common/client/gui/controls/VisualTree.h>
#include <mc/common/client/gui/controls/UIControl.h>

Chat::Chat() : HUDModule("Chat", L"Custom Chat", L"A custom chat, replacing the vanilla chat.", HUD) {
	listen<ChatMessageEvent>((EventListenerFunc)&Chat::onText);
	//listen<LatiteClientMessageEvent>((EventListenerFunc)&Chat::onLatiteMessage);
	//listen<RenderLayerEvent>((EventListenerFunc)&Chat::onRenderLayer, true);
	
	anchorData.addEntry(EnumEntry{ anchor_auto, L"Auto" });
	anchorData.addEntry(EnumEntry{ anchor_top,L"Top" });
	anchorData.addEntry(EnumEntry{ anchor_bottom, L"Bottom" });

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

	addSliderSetting("chatWidth", L"Chat Width", L"", chatWidth, FloatValue(100.f), FloatValue(1000.f), FloatValue(10.f));
	addSetting("backgroundColor", L"Background Color", L"The color of the background", backgroundColor);
	addSetting("textColor", L"Text Color", L"The color of the text", textColor);
	addSetting("antiSpam", L"Anti Spam", L"Prevent spam messages in chat.", antiSpam);
	addSliderSetting("animSpeed", L"Animation Time", L"The speed of the animation", animationSpeed, FloatValue(0.f), FloatValue(5.f), FloatValue(0.1f));
	addSliderSetting("messageDuration", L"Message Duration", L"The duration of the message, excluding the animation", messageDuration, FloatValue(1.f), FloatValue(15.f), FloatValue(0.5f));
	addEnumSetting("anchor", L"Anchor", L"How the chat window will be anchored", anchorData);
	
	showPreview = false;
}

void Chat::render(DrawUtil& ctx, bool isDefault, bool inEditor) {
	d2d::Color bgCol = std::get<ColorValue>(backgroundColor).getMainColor();
	d2d::Color fgCol = std::get<ColorValue>(textColor).getMainColor();

	float windowWidth = std::get<FloatValue>(chatWidth);
	float windowHeight = messageHeight * maxMessages;
	auto tm = std::chrono::system_clock::now();

	auto messageDuration = std::chrono::milliseconds(static_cast<int64_t>(std::get<FloatValue>(this->messageDuration).value * 1000));
	auto messageAnimDuration = std::chrono::milliseconds(static_cast<int64_t>(std::get<FloatValue>(this->animationSpeed).value * 1000));


	for (auto& msg : messages) {
		if (tm - msg.timeCreated >= (messageDuration + messageAnimDuration)) {
			messages.pop_back();
		}
	}

	float y = windowHeight;
	for (size_t i = 0; i < std::min(messages.size(), static_cast<size_t>(maxMessages)); i++) {
		auto& msg = messages[i];

		auto textWrap = [](DrawUtil& util, std::wstring txt, float tSize, float width) -> std::wstring {
			std::wstring ret = txt;
			std::wstring work = ret;

			size_t retIndex = 1;
			for (size_t i = retIndex; i < work.size(); i++) {
				float size = util.getTextSize(work.substr(0, i), Renderer::FontSelection::PrimaryRegular, tSize).x;
				if (size > width) {
					ret.insert(ret.begin() + (retIndex - 1), L'\n');
					i = 1;
					work = ret.substr(retIndex);
				}
				retIndex++;
			}

			return ret;
			};

		auto sineCurve = [](float x) {
			return sin(x * 0.5f * pi_f);
		};

		if (tm - msg.timeCreated > messageDuration) {
			msg.animation = sineCurve(std::min(1.f, (messageAnimDuration.count() / (float)std::chrono::duration_cast<std::chrono::milliseconds>(tm - msg.timeCreated - messageDuration).count()) - 1.f));
		}
		else if (tm - msg.timeCreated < messageAnimDuration && msg.duplicate < 2) {
			msg.animation = sineCurve((float)std::chrono::duration_cast<std::chrono::milliseconds>(tm - msg.timeCreated).count() / std::chrono::duration_cast<std::chrono::milliseconds>(messageAnimDuration).count());
		}
		else {
			msg.animation = 1.f;
		}

		std::string content = msg.content;
		if (msg.duplicate > 1) {
			content += " \xC2\xA7\x37[" + std::to_string(msg.duplicate) + "]";
		}

		std::wstring text = textWrap(ctx, util::StrToWStr(content), textSize, windowWidth);
		auto tSize = ctx.getTextSize(text, Renderer::FontSelection::PrimaryRegular, textSize);

		d2d::Rect tRect = { 0, y - tSize.y,
			windowWidth, y};

		ctx.fillRectangle(tRect, bgCol.asAlpha(msg.animation * bgCol.a));

		tRect = { (1.f - msg.animation) * -windowWidth, y - tSize.y,
			windowWidth * (msg.animation), y};

		y -= tSize.y;

		/*we will cache the message in d2d. it's more worth it*/
		ctx.drawText(tRect, text, fgCol.asAlpha(msg.animation * fgCol.a), Renderer::FontSelection::PrimaryRegular, textSize,
			DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, true);
	}

	rect.right = rect.left + windowWidth;
	rect.bottom = rect.top + windowHeight;
}

void Chat::onText(Event& evG) {
	auto& ev = reinterpret_cast<ChatMessageEvent&>(evG);
	//auto pkt = ev.getTextPacket();

	//std::string content = pkt->str;
	//if (pkt->source.textSize > 0) {
	//	content = "<" + pkt->source.str() + "> " + content;
	//}

	addMessage(ev.getMessage());
	ev.setCancelled(true);
}

void Chat::onLatiteMessage(Event& evG) {
	auto& ev = reinterpret_cast<LatiteClientMessageEvent&>(evG);
	addMessage(ev.getMessage());
}

void Chat::onRenderLayer(Event& evG) {
	// TODO: This method is very scuffed. A better method is to actually disable the rendering of the chat or to make the control invisible.
	
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);

	if (ev.getScreenView()->visualTree->rootControl->name == XOR_STRING("hud_screen")) {
		//static bool lastEnabled = false;

		//if (isEnabled() != lastEnabled) {
		chatStack = ev.getScreenView()->visualTree->rootControl->findFirstDescendantWithName(XOR_STRING("chat_panel"));
		if (isEnabled()) {
			if (chatStack->position.x < 20000) {
				chatStack->position.x += 20000;
			}
		}
		else if (chatStack->position.x > 20000) {
			chatStack->position.x -= 20000;
		}
		chatStack->getDescendants([](std::shared_ptr<SDK::UIControl> control) {
			control->updatePos();
			});

		//}
		chatStack = nullptr;
	}
}

void Chat::addMessage(std::string const& message) {
	if (std::get<BoolValue>(antiSpam)) {
		for (size_t i = 0; i < messages.size(); i++) {
			if (messages[i].content == message) {
				messages[i].duplicate++;
				messages[i].timeCreated = std::chrono::system_clock::now();

				// Put the message in the front of the message list.
				std::swap(messages[i], messages[0]);
				return;
			}
		}
	}

	messages.emplace(messages.begin(), message);
}
