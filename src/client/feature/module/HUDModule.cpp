#include "HUDModule.h"
#include "util/Util.h"

void HUDModule::renderSelected() {
	DXContext dc;
	dc.fillRectangle(this->getRect(), { 1.f, 1.f, 1.f, 0.2f });
}

void HUDModule::renderFrame()
{
	DXContext ctx;
	ctx.fillRectangle(this->getRect(), { 0.F, 0.F, 0.F, 0.4F });
	ctx.drawRectangle(this->getRect(), d2d::Colors::BLACK, 0.6f);
}

void HUDModule::renderPost()
{
	DXContext ctx;
	ctx.drawText(rect, util::StrToWStr(this->getDisplayName()), d2d::Color(0.5F, 1.0F, 1.0F, 1.f), Renderer::FontSelection::Light, 
		16.f);
}
