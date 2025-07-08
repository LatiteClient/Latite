#include "pch.h"
#include "EnvironmentChanger.h"
#include <client/event/events/WeatherEvent.h>

EnvironmentChanger::EnvironmentChanger() : Module("EnvironmentChanger", LocalizeString::get("client.module.environmentChanger.name"),
                                                  LocalizeString::get("client.module.environmentChanger.desc"), GAME) {
	listen<WeatherEvent>(static_cast<EventListenerFunc>(&EnvironmentChanger::onWeather));
	listen<FogColorEvent>(static_cast<EventListenerFunc>(&EnvironmentChanger::onFogColor));
	listen<GetTimeEvent>(static_cast<EventListenerFunc>(&EnvironmentChanger::onTime));

    addSetting("setFogColor", LocalizeString::get("client.module.environmentChanger.setFogColor.name"), L"",
               setFogColor);

	addSetting("fogColor", LocalizeString::get("client.module.environmentChanger.fogColor.name"),
               LocalizeString::get("client.module.environmentChanger.fogColor.desc"), fogColor, "setFogColor"_istrue);

	addSetting("setTime", LocalizeString::get("client.module.environmentChanger.setTime.name"),
               LocalizeString::get("client.module.environmentChanger.setTime.desc"), setTime);

	addSliderSetting("timeToSet", LocalizeString::get("client.module.environmentChanger.timeToSet.name"),
                     LocalizeString::get("client.module.environmentChanger.timeToSet.desc"), time, FloatValue(0.f),
                     FloatValue(1.f), FloatValue(0.01f), "setTime");

	addSetting("showWeather", LocalizeString::get("client.module.environmentChanger.showWeather.name"),
               LocalizeString::get("client.module.environmentChanger.showWeather.desc"), showWeather);
}

void EnvironmentChanger::onWeather(Event& evG) {
	auto& ev = reinterpret_cast<WeatherEvent&>(evG);

	if (!std::get<BoolValue>(showWeather)) {
		ev.setShowWeather(false);
	}
}

void EnvironmentChanger::onFogColor(Event& evG) {
	auto& ev = reinterpret_cast<FogColorEvent&>(evG);
	if (std::get<BoolValue>(setFogColor)) {
		auto col = std::get<ColorValue>(fogColor).getMainColor();
		ev.setColor(col.r, col.g, col.b);
	}
}

void EnvironmentChanger::onTime(Event& evG) {
	auto& ev = reinterpret_cast<GetTimeEvent&>(evG);

	if (std::get<BoolValue>(setTime)) {
		ev.setTime(std::get<FloatValue>(time));
	}
}
