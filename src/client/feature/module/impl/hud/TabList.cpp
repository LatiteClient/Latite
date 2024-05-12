#include "pch.h"
#include "TabList.h"
#include "client/Latite.h"
#include "client/render/Assets.h"

TabList::TabList() : Module("PlayerList", "Player List", "Shows the player list.", HUD, VK_TAB) {
	
	addSetting("textColor", "Text", "Text Color", textCol);
	addSetting("bgColor", "Background", "Background Color", bgCol);
	listen<RenderOverlayEvent>((EventListenerFunc)&TabList::onRenderOverlay);
}

void TabList::onRenderOverlay(Event& evG) {
	auto& ev = static_cast<RenderOverlayEvent&>(evG);

	auto plr = SDK::ClientInstance::get()->getLocalPlayer();
	if (!plr) return;

	D2DUtil dc;
	auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();

	size_t size = lvl->getPlayerList()->size();

	float textP = 20.f;

	std::wstring txt;
	if (SDK::RakNetConnector::get() && SDK::RakNetConnector::get()->featuredServer.size() > 0) {
		txt = util::StrToWStr(SDK::RakNetConnector::get()->featuredServer);
	}
	else {
		txt = util::StrToWStr(lvl->name);
	}

	constexpr auto font = Renderer::FontSelection::PrimaryRegular;
	float sectionHeight = textP * 1.3f;

	float logoSize = sectionHeight;
	float logoPad = 4.f;

	float longestText = dc.getTextSize(txt, font, textP).x;
	for (auto& ent : *lvl->getPlayerList()) {
		auto w = dc.getTextSize(util::StrToWStr(ent.second.name), font, textP).x + 3.f;
		auto const& name = util::StrToWStr(ent.second.name);
		for (auto& user : Latite::get().getLatiteUsers()) {
			if (user == ent.second.name) {
				w += logoPad + logoSize;
			}
		}

		if (w > longestText) longestText = w;
	}

	float sectionSize = longestText;

	size_t maxPerTab = 15;
	int numTabs = static_cast<int>(std::ceil(static_cast<float>(size) / static_cast<float>(maxPerTab)));

	float oY = sectionHeight;

	float x = 0.f;
	float y = oY;

	size_t idx = 0;


	float calcWidth = sectionSize * static_cast<float>(numTabs);
	float calcHeight = sectionHeight * ((static_cast<float>(size) > maxPerTab) ? maxPerTab : static_cast<float>(size));


	auto& ss = SDK::ClientInstance::get()->getGuiData()->screenSize;
	D2D1::Matrix3x2F mat;
	dc.ctx->GetTransform(&mat);
	dc.ctx->SetTransform(mat * D2D1::Matrix3x2F::Translation(ss.x / 2.f - calcWidth / 2.f, 20.f));


	dc.fillRectangle({ 0.f, 0.f, calcWidth, calcHeight + oY }, std::get<ColorValue>(this->bgCol).color1);
	dc.drawRectangle({ 0.f, 0.f, calcWidth, calcHeight + oY }, d2d::Color(std::get<ColorValue>(this->bgCol).color1).asAlpha(1.f), 2.f);

	dc.drawText({ 0.f, 0.f, calcWidth, oY }, txt, std::get<ColorValue>(this->textCol).color1, font, textP, DWRITE_TEXT_ALIGNMENT_CENTER);


	for (auto& ent : *lvl->getPlayerList()) {
		auto const& name = util::StrToWStr(ent.second.name);
		d2d::Rect rc = { x, y, x + longestText, y + sectionHeight };
		for (auto& user : Latite::get().getLatiteUsers()) {
			if (user == ent.second.name) {
				rc.left += logoSize + logoPad;
				d2d::Rect logoRc = { x, y, x + logoSize, y + logoSize };
				dc.ctx->DrawBitmap(Latite::getAssets().logoWhite.getBitmap(), logoRc);
			}
		}

		// render
		//dc.drawRectangle(rc, d2d::Colors::BLACK, 0.5f);

		dc.drawText(rc, name, std::get<ColorValue>(textCol).color1, font, textP, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);

		idx++;
		if (idx < maxPerTab) {
			y += sectionHeight;
			continue;
		}
		x += longestText;
		y = oY;
		idx = 0;
	}
	dc.ctx->SetTransform(mat);
}
