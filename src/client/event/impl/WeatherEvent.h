#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class WeatherEvent : public Event {
public:
	static const uint32_t hash = TOHASH(WeatherEvent);

	WeatherEvent() {
	}

	void setShowWeather(bool showWeather) {
		this->showWeather = showWeather;
	}

	bool shouldShowWeather() {
		return showWeather;
	}
protected:
	bool showWeather = true;
};
