#include "pch.h"
#include "../../resource/Resource.h"
#include "Asset.h"

#include <Shlwapi.h>   // SHCreateMemStream

IStream* CreateStreamFromBuffer(const uint8_t* begin, const uint8_t* end) {
	size_t size = end - begin;

	// SHCreateMemStream takes BYTE*, UINT
	return SHCreateMemStream(reinterpret_cast<const BYTE*>(begin),
							 static_cast<UINT>(size));
}

void Asset::load(IWICImagingFactory* factory, ID2D1DeviceContext* dc) {
	ComPtr<IWICBitmapDecoder> pDecoder = NULL;

	auto stream = ComPtr<IStream>(SHCreateMemStream(reinterpret_cast<const BYTE*>(resource.begin()),
		resource.end() - resource.begin()));


	factory->CreateDecoderFromStream(
		stream.Get(),
		nullptr,// Do not prefer a particular vendor
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

	ThrowIfFailed(dc->CreateBitmapFromWicBitmap(conv.Get(), nullptr, &bitmap));
}

void Asset::unload() {
	this->bitmap = nullptr;
}
