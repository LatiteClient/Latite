#include "pch.h"
#include "Assets.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"

Assets::Assets() {
	allAssets.push_back(&this->latiteLogo);
	allAssets.push_back(&this->searchIcon);
	allAssets.push_back(&this->arrowIcon);
	allAssets.push_back(&this->xIcon);
	allAssets.push_back(&this->hudEditIcon);
	allAssets.push_back(&this->arrowBackIcon);
	allAssets.push_back(&this->cogIcon);
	allAssets.push_back(&this->checkmarkIcon);
	allAssets.push_back(&this->logoWhite);
	//allAssets.push_back(&this->document);
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
