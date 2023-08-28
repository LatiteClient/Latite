#include "pch.h"
#include "Asset.h"

Asset::Asset(std::wstring const& relPath) : relPath(relPath) {
}

void Asset::load(IWICImagingFactory* factory, ID2D1DeviceContext* dc) {
	auto path = util::GetLatitePath() / "Assets" / relPath;

	ComPtr<IWICBitmapDecoder> pDecoder = NULL;

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

	ThrowIfFailed(dc->CreateBitmapFromWicBitmap(conv.Get(), NULL, &bitmap));
}

void Asset::unload() {
	this->bitmap = nullptr;
}
