#include "pch.h"
#include "MotionBlur.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"

#include "client/event/impl/RendererInitEvent.h"

MotionBlur::MotionBlur() : Module("MotionBlur", "Motion Blur", "Blurs motion", GAME) {
	addSliderSetting("intensity", "Intensity", "", intensity, FloatValue(0.f), FloatValue(20.f), FloatValue(1.f));
	
	listen<RendererCleanupEvent>(&MotionBlur::onCleanup);
	listen<RenderOverlayEvent>(&MotionBlur::onRender, false, 10);
	listen<RendererInitEvent>(&MotionBlur::onRendererInit, true);

}

void MotionBlur::onEnable() {
}

void MotionBlur::onDisable() {
}

void MotionBlur::onRender(Event& genericEv) {
	auto& ev = reinterpret_cast<RenderOverlayEvent&>(genericEv);
	auto ctx = ev.getDeviceContext();

	ctx->Flush();
	this->motionBlurList.push_back(Latite::getRenderer().copyCurrentBitmap());
	size_t factor = static_cast<size_t>(std::floor(std::get<FloatValue>(intensity)));
	if (motionBlurList.size() > factor) {
		for (size_t i = 0; i < motionBlurList.size(); i++) {
			ctx->DrawBitmap(motionBlurList[i], nullptr, 0.098f);
		}
		motionBlurList[0]->Release();
		motionBlurList.erase(motionBlurList.begin());
	}

	if (motionBlurList.size() > (factor + 1)) {
		for (auto& mb : motionBlurList) {
			mb->Release();
		}
		motionBlurList.clear();
	}

	/*
	oBitmap->CopyFromBitmap(nullptr, Latite::getRenderer().getBitmap(), nullptr);
	ctx->DrawBitmap(mbBitmap, nullptr, 0.9f);
	//ctx->DrawBitmap(oBitmap, nullptr, 0.2f); // bleed fix

	mbBitmap->CopyFromBitmap(nullptr, Latite::getRenderer().getBitmap(), nullptr);
	*/
}

void MotionBlur::onCleanup(Event&) {
	//auto& ev = reinterpret_cast<RendererCleanupEvent&>(genericEv);
	SafeRelease(&mbBitmap);
	SafeRelease(&oBitmap);
}

void MotionBlur::onRendererInit(Event& genericEv) {
	this->mbBitmap = Latite::getRenderer().copyCurrentBitmap();
	this->oBitmap = Latite::getRenderer().copyCurrentBitmap();
}
