#include "pch.h"
#include "JsTexture.h"
#include "client/Latite.h"

JsTexture::JsTexture(std::wstring const& textureNameOrPath, bool gameTexture) {
	this->nameOrPath = textureNameOrPath;
	this->gameTexture = gameTexture;
	this->loadMinecraft();
	auto path = this->tryGetRealPath(textureNameOrPath);

	if (gameTexture) return;

	// FIXME: same as above
	Latite::get().queueForDXRender([this, path](ID2D1DeviceContext* ctx) {

		ComPtr<IWICBitmapDecoder> pDecoder = NULL;

		auto factory = Latite::getRenderer().getImagingFactory();
		// i love repeating code!!!!
		auto res = factory->CreateDecoderFromFilename(
			path.wstring().c_str(),
			nullptr,// Do not prefer a particular vendor
			GENERIC_READ,                    // Desired read access to the file
			WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
			pDecoder.GetAddressOf()                        // Pointer to the decoder
		);

		// Retrieve the first frame of the image from the decoder
		ComPtr<IWICBitmapFrameDecode> pFrame;

		pDecoder->GetFrame(0, pFrame.GetAddressOf());

		ComPtr<IWICFormatConverter> conv;
		factory->CreateFormatConverter(conv.GetAddressOf());
		conv->Initialize(
			pFrame.Get(),
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.0,
			WICBitmapPaletteTypeCustom);

		if (FAILED(ctx->CreateBitmapFromWicBitmap(conv.Get(), NULL, this->d2dTexture.GetAddressOf()))) {
			this->failed = true;
		}
		});
}

void JsTexture::loadMinecraft() {
	mcTexture = SDK::TexturePtr{};
	if (gameTexture) {
		// FIXME: I sure hope this object doesn't get destroyed by the time this function calls
		Latite::get().queueForUIRender([this](SDK::MinecraftUIRenderContext* ctx) {
			ctx->getTexture(&this->mcTexture.value(), SDK::ResourceLocation(util::WStrToStr(nameOrPath), 0 /*0 = default minecraft texture*/), false /*not external*/);
			});
		return;
	}

	auto path = this->tryGetRealPath(nameOrPath);
	// FIXME: I sure hope this object doesn't get destroyed by the time this function calls
	Latite::get().queueForUIRender([this, path](SDK::MinecraftUIRenderContext* ctx) {
		ctx->getTexture(&this->mcTexture.value(), SDK::ResourceLocation(util::WStrToStr(path), 2 /*2 = external texture*/), true /*external*/);
		});
}

void JsTexture::reloadMinecraft() {
	// very funny code
	if (mcTexture.has_value()) {
		mcTexture->~TexturePtr();
	}
	mcTexture = std::nullopt;

	this->loadMinecraft();
}

std::filesystem::path JsTexture::tryGetRealPath(std::wstring const& oPath) {
	auto scriptPath = std::filesystem::path(JsScript::getThis()->indexPath).parent_path();
	if (std::filesystem::exists(scriptPath / oPath)) return scriptPath / oPath;
	return oPath;
}
