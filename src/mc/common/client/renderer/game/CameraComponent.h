#pragma once
namespace SDK {
	class CameraComponent
	{
		char pad_0000[48]; //0x0000
	public:
		Vec4 lookAngles; //0x0030
		Vec3 cameraPos; //0x0040
		Vec2 fov; //0x004C
		float nearClippingPane; //0x0054
		float farClippingPane; //0x0058
	}; //Size: 0x0088
}