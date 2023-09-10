#include "pch.h"
#include "HUDModule.h"
#include <client/Latite.h>

void HUDModule::renderSelected() {
	D2DUtil dc;
	dc.fillRectangle(this->getRect(), { 1.f, 1.f, 1.f, 0.2f });
}

void HUDModule::renderFrame()
{
	D2DUtil ctx;
	ctx.fillRectangle(this->getRect(), { 0.F, 0.F, 0.F, 0.4F });
	ctx.drawRectangle(this->getRect(), d2d::Colors::BLACK, 0.6f);
}

void HUDModule::renderPost()
{
	D2DUtil ctx;
	ctx.drawText(getRect(), util::StrToWStr(this->getDisplayName()), d2d::Color(0.5F, 1.0F, 1.0F, 1.f), Renderer::FontSelection::SegoeLight,
		16.f);
}

void HUDModule::afterLoadConfig() {
	auto ss = Latite::getRenderer().getScreenSize();
	auto& sp = std::get<Vec2Value>(this->storedPos);
	setPos({ sp.x * ss.width, sp.y * ss.height });
}

void HUDModule::storePos() {
	auto& vec = std::get<Vec2Value>(storedPos);
	auto& ss = SDK::ClientInstance::get()->getGuiData()->screenSize;
	vec.x = rect.left / ss.x;
	vec.y = rect.top / ss.y;
}

d2d::Rect HUDModule::getRect() {
	return { rect.left, rect.top, rect.left + ((rect.getWidth()) * getScale()), rect.top + ((rect.getHeight()) * getScale()) };
}
