#include "ClickGUI.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RendererCleanupEvent.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"
#include "util/Util.h"
#include "util/DxContext.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "client/render/Assets.h"

using FontSelection = Renderer::FontSelection;
using RectF = d2d::Rect;

ClickGUI::ClickGUI() : Screen("ClickGUI") {
	Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&ClickGUI::onRender);
	Eventing::get().listen<RendererCleanupEvent>(this, (EventListenerFunc)&ClickGUI::onCleanup);
}

void ClickGUI::onRender(Event& evGeneric) {
	sdk::ClientInstance::get()->releaseCursor();

	//auto& ev = reinterpret_cast<RenderOverlayEvent&>(evGeneric);
	auto& rend = Latite::getRenderer();
	auto ss = rend.getDeviceContext()->GetSize();

	float guiX = ss.width / 4.f;
	float guiY = ss.height / 4.f;

	DXContext dc;

	{
		RECT desktopSize;
		HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktopSize);
		float width = static_cast<float>(desktopSize.right - desktopSize.left);
		float height = static_cast<float>(desktopSize.bottom - desktopSize.top);

		float realGuiX = width / 4.f;
		float realGuiY = height / 4.f;

		float leewayX = (width / 2) - realGuiX;
		float leewayY = (height / 2) - realGuiY;

		guiX = ((ss.width / 2.f) - (leewayX));
		guiY = ((ss.height / 2.f) - (leewayY));
	}

	RectF rect = { guiX, guiY, ss.width - guiX, ss.height - guiY };
	d2d::Color outline = d2d::Color::RGB(0, 0, 0);
	outline.a = 0.28f;
	d2d::Color rcColor = d2d::Color::RGB(0x7, 0x7, 0x7);
	rcColor.a = 0.75f;
	rect.round();


	dc.drawGaussianBlur(20.f);
	dc.fillRoundedRectangle(rect, rcColor, 19.f);
	dc.drawRoundedRectangle(rect, outline, 19.f, 4.f, DXContext::OutlinePosition::Outside);
	dc.setFont(FontSelection::Semilight);
	dc.drawText({ 0, 0, 200, 30 }, "Hello world!", D2D1::ColorF::White, 30.f, DWRITE_TEXT_ALIGNMENT_CENTER);

	D2D1_RECT_F logoRect = { rect.left + 16.f, rect.top + 18.f, rect.left + 16.f + 41.f, rect.top + 18.f + 41.f };

	{
		auto bmp = Latite::getAssets().latiteLogo.getBitmap();
		// I dont know why this doenst work :sob:
		auto shadowEffect = Latite::getRenderer().getShadowEffect();
		shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::ColorF(0.f, 0.f, 0.f, 0.3f));
		shadowEffect->SetInput(0, bmp);
		auto affineTransformEffect = Latite::getRenderer().getAffineTransformEffect();
		ComPtr<ID2D1Effect> composite;
		dc.ctx->CreateEffect(CLSID_D2D1Composite, composite.GetAddressOf());

		D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Translation(30, 30);
		affineTransformEffect->SetInputEffect(0, shadowEffect);
		affineTransformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, mat);

		composite->SetInputEffect(0, affineTransformEffect);
		composite->SetInput(1, bmp);
		//dc.ctx->DrawBitmap(bmp, logoRect, 1.f);
		D2D1::Matrix3x2F oMat;
		auto sz = bmp->GetSize();

		D2D1::Matrix3x2F m;
		dc.ctx->GetTransform(&m);
		dc.ctx->SetTransform(D2D1::Matrix3x2F::Scale(41.f / sz.width, 41.f / sz.height) * D2D1::Matrix3x2F::Translation(logoRect.left, logoRect.top) * m);
		dc.ctx->DrawImage(composite.Get(), { 0.f, 0.f });
		dc.ctx->SetTransform(m);
	}

	dc.setFont(FontSelection::Light);
	dc.drawText({ logoRect.right + 10.f, logoRect.top + 4.f, logoRect.right + 500.f, logoRect.top + 50.f }, "Latite Client", d2d::Color(1.f, 1.f, 1.f, 1.f), 25.f, DWRITE_TEXT_ALIGNMENT_LEADING);
}

void ClickGUI::onCleanup(Event& ev) {
}
