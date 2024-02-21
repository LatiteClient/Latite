#include "pch.h"
#include "HUDModule.h"
#include <client/Latite.h>

void HUDModule::renderSelected(DrawUtil& ctx) {
	ctx.fillRectangle(this->getRect(), { 1.f, 1.f, 1.f, 0.2f });
}

void HUDModule::renderFrame(DrawUtil& ctx) {
	ctx.fillRectangle(this->getRect(), { 0.F, 0.F, 0.F, 0.4F });
	ctx.drawRectangle(this->getRect(), d2d::Colors::BLACK, 1.5f);
}

void HUDModule::renderPost(DrawUtil& ctx) {
	ctx.drawText(getRect(), util::StrToWStr(this->getDisplayName()), d2d::Color(0.5F, 1.0F, 1.0F, 1.f), Renderer::FontSelection::SegoeLight,
		16.f);
}

void HUDModule::afterLoadConfig() {
	auto ss = SDK::ClientInstance::get()->getGuiData()->screenSize;//Latite::getRenderer().getScreenSize();
	auto& sp = std::get<Vec2Value>(this->storedPos);
	setPos({ sp.x * ss.x, sp.y * ss.y });
}

void HUDModule::storePos(Vec2 const& ss) {
	auto& vec = std::get<Vec2Value>(storedPos);
	vec.x = rect.left / ss.x;
	vec.y = rect.top / ss.y;
}

d2d::Rect HUDModule::getRect() {
	return { rect.left, rect.top, rect.left + ((rect.getWidth()) * getScale()), rect.top + ((rect.getHeight()) * getScale()) };
}

void HUDModule::loadConfig(SettingGroup& resolvedGroup) {
	Module::loadConfig(resolvedGroup);
}
