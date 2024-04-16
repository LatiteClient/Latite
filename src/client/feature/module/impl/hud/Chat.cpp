#include "pch.h"
#include "Chat.h"
#include <sdk/common/client/gui/ScreenView.h>
#include <sdk/common/client/gui/controls/VisualTree.h>
#include <sdk/common/client/gui/controls/UIControl.h>

Chat::Chat() : HUDModule("Chat", "Custom Chat", "A custom chat, replacing the vanilla chat.", HUD) {
	listen<ClientTextEvent>((EventListenerFunc)&Chat::onText);
	listen<LatiteClientMessageEvent>((EventListenerFunc)&Chat::onLatiteMessage);
	listen<RenderLayerEvent>((EventListenerFunc)&Chat::onRenderLayer, true);
	
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
	addSetting("antiSpam", "Anti Spam", "Prevent spam messages in chat.", antiSpam);
	addEnumSetting("anchor", "Anchor", "How the chat window will be anchored", anchorData);
}

void Chat::render(DrawUtil& ctx, bool isDefault, bool inEditor) {
	d2d::Color bgCol = std::get<ColorValue>(backgroundColor).color1;
	d2d::Color fgCol = std::get<ColorValue>(textColor).color1;

	float windowWidth = std::get<FloatValue>(chatWidth);
	float windowHeight = messageHeight * maxMessages;
	auto tm = std::chrono::system_clock::now();

	static constexpr auto messageDuration = 6s;
	static constexpr auto messageAnimDuration = 1s;

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
				float size = util.getTextSize(work.substr(0, i), Renderer::FontSelection::SegoeRegular, tSize).x;
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
			msg.animation = sineCurve(std::min(1.f, (1000.f / (float)std::chrono::duration_cast<std::chrono::milliseconds>(tm - msg.timeCreated - messageDuration).count()) - 1.f));
		}
		else if (tm - msg.timeCreated < messageAnimDuration && msg.animation < 0.99f) {
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
		auto tSize = ctx.getTextSize(text, Renderer::FontSelection::SegoeRegular, textSize);

		d2d::Rect tRect = { 0, y - tSize.y,
			windowWidth, y};

		ctx.fillRectangle(tRect, bgCol.asAlpha(msg.animation * bgCol.a));

		tRect = { (1.f - msg.animation) * -windowWidth, y - tSize.y,
			windowWidth * (msg.animation), y};

		y -= tSize.y;

		/*we will cache the message in d2d. it's more worth it*/
		ctx.drawText(tRect, text, fgCol.asAlpha(msg.animation * fgCol.a), Renderer::FontSelection::SegoeRegular, textSize,
			DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, true);
	}

	rect.right = rect.left + windowWidth;
	rect.bottom = rect.top + windowHeight;
}

void Chat::onText(Event& evG) {
	auto& ev = reinterpret_cast<ClientTextEvent&>(evG);
	auto pkt = ev.getTextPacket();

	std::string content = pkt->str;
	if (pkt->source.textSize > 0) {
		content = "<" + pkt->source.str() + "> " + content;
	}

	addMessage(content);
}

void Chat::onLatiteMessage(Event& evG) {
	auto& ev = reinterpret_cast<LatiteClientMessageEvent&>(evG);
	addMessage(ev.getMessage());
}

void Chat::onRenderLayer(Event& evG) {
	// TODO: This method is very scuffed. A better method is to actually disable the rendering of the chat or to make the control invisible.
	
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);

	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		chatStack = nullptr;
		return;
	}

	if (ev.getScreenView()->visualTree->rootControl->name == XOR_STRING("hud_screen")) {
		static bool lastEnabled = false;

		if (isEnabled() != lastEnabled) {
			if (!chatStack) chatStack = ev.getScreenView()->visualTree->rootControl->findFirstDescendantWithName(XOR_STRING("chat_stack"));
			if (isEnabled()) {
				chatStack->position.x = SDK::ClientInstance::get()->getGuiData()->guiSize.x + 1000.f;
			}
			else {
				chatStack->position.x = 0.f;
			}
			chatStack->getDescendants([](std::shared_ptr<SDK::UIControl> control) {
				control->updatePos();
				});

			lastEnabled = isEnabled();
		}
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
