#include "pch.h"
#include "../../resource/Resource.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"
#include "client/render/asset/Assets.h"
#include "../../resource/InitResources.h"

Assets::Assets() :
	latiteLogo(GET_RESOURCE(logo_png)),
	searchIcon(GET_RESOURCE(searchicon_png)),
	arrowIcon(GET_RESOURCE(arrow_png)),
	xIcon(GET_RESOURCE(x_png)),
	hudEditIcon(GET_RESOURCE(hudedit_png)),
	arrowBackIcon(GET_RESOURCE(arrow_back_png)),
	cogIcon(GET_RESOURCE(cog_png)),
	checkmarkIcon(GET_RESOURCE(checkmark_png)),
	logoWhite(GET_RESOURCE(latitewhite_png))

{

	allAssets.push_back(&this->latiteLogo);
	allAssets.push_back(&this->searchIcon);
	allAssets.push_back(&this->arrowIcon);
	allAssets.push_back(&this->xIcon);
	allAssets.push_back(&this->hudEditIcon);
	allAssets.push_back(&this->arrowBackIcon);
	allAssets.push_back(&this->cogIcon);
	allAssets.push_back(&this->checkmarkIcon);
	allAssets.push_back(&this->logoWhite);
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
