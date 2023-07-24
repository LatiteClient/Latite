#include "Assets.h"
#include "client/Latite.h"
#include "Renderer.h"

Assets::Assets() {
	allAssets.push_back(&this->latiteLogo);
	allAssets.push_back(&this->searchIcon);
	allAssets.push_back(&this->arrowIcon);
}

void Assets::loadAll() {
	for (auto& asset : allAssets) {
		asset->load(Latite::getRenderer().getImagingFactory(), Latite::getRenderer().getDeviceContext());
	}
}

void Assets::unloadAll() {
	for (auto& asset : allAssets) {
		asset->unload();
	}
}
