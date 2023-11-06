#include "pch.h"
#include "JsTexture.h"
#include "client/Latite.h"

JsTexture::JsTexture(std::wstring const& textureNameOrPath, bool isMinecraft) {
	this->mcTexture = SDK::TexturePtr{};
	if (isMinecraft) {

		// FIXME: I sure hope this object doesn't get destroyed by the time this function calls
		Latite::get().queueForUIRender([this, textureNameOrPath](SDK::MinecraftUIRenderContext* ctx) {
			ctx->getTexture(SDK::ResourceLocation(util::WStrToStr(textureNameOrPath), 0 /*0 = default minecraft texture*/), false /*not external*/);
			});
	}

	auto path = this->tryGetRealPath(textureNameOrPath);
	// FIXME: I sure hope this object doesn't get destroyed by the time this function calls
	Latite::get().queueForUIRender([this, path](SDK::MinecraftUIRenderContext* ctx) {
		ctx->getTexture(SDK::ResourceLocation(util::WStrToStr(path), 0 /*0 = default minecraft texture*/), false /*not external*/);
		});

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

std::filesystem::path JsTexture::tryGetRealPath(std::wstring const& oPath) {
	auto scriptPath = std::filesystem::path(JsScript::getThis()->indexPath).parent_path();
	if (std::filesystem::exists(scriptPath / oPath)) return scriptPath / oPath;
	return oPath;
}
