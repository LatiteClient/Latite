#pragma once
#include <cstdint>

namespace SDK {
	class Weather {
		char pad_0008[40]; //0x0008
	public:
		struct WeatherData {
			float rainTransitionOld{}; //0x0034
			float rainTransition{}; //0x0038
			float rain{}; //0x003C
			float thunderTransitionOld{}; //0x0040
			float thunderTransition{}; //0x0044
			float thunder{}; //0x0048
			float skyDarkness{}; //0x004C

			WeatherData() = default;
		};

		int32_t tick; //0x0030
		WeatherData data; //0x0034
	private:
		virtual void Function0();
	};
}