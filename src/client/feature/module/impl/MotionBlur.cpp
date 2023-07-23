#include "MotionBlur.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"

MotionBlur::MotionBlur() : Module("MotionBlur", "Motion Blur", "Blurs motion") {
}

void MotionBlur::onEnable() {
}

void MotionBlur::onDisable() {
}

void MotionBlur::onRender(Event& genericEv) {
	auto& ev = reinterpret_cast<RenderOverlayEvent&>(genericEv);
	auto ctx = ev.getDeviceContext();

	if (mbBitmap) {
		SafeRelease(&mbBitmap);
	}

	mbBitmap = Latite::getRenderer().copyCurrentBitmap();
}

void MotionBlur::onCleanup(Event& genericEv) {
	//auto& ev = reinterpret_cast<RendererCleanupEvent&>(genericEv);
	if (mbBitmap) {
		SafeRelease(&mbBitmap);
	}
}
