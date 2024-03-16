#include "../../Module.h"

class EnvironmentChanger : public Module {
public:
    EnvironmentChanger();

    void onWeather(Event&);
    void onFogColor(Event&);
    void onTime(Event&);

private:
    ValueType setFogColor = BoolValue(true);
    ValueType fogColor = ColorValue{ 1.f, 0.f, 0.f, 1.f };

    ValueType setTime = BoolValue(false);
    ValueType time = FloatValue(0.f);

    ValueType showWeather = BoolValue(true);
};