#include "pch.h"
#include "GuiscaleChanger.h"
#include "client/event/impl/TickEvent.h"

GuiscaleChanger::GuiscaleChanger() : Module("GUIScaleChanger",
                                            LocalizeString::get("client.module.guiScaleChanger.name"),
                                            LocalizeString::get("client.module.guiScaleChanger.desc"), HUD,
                                            nokeybind) {
    this->listen<TickEvent>(&GuiscaleChanger::onTick);
    addSliderSetting("guiscale", LocalizeString::get("client.module.guiScaleChanger.guiscale.name"),
                     LocalizeString::get("client.module.guiScaleChanger.guiscale.desc"), this->guiscale,
                     FloatValue(0.5f), FloatValue(5.f), FloatValue(0.5f));
}

void GuiscaleChanger::onTick(Event& evGeneric)
{
    SDK::ClientInstance::get()->getGuiData()->updateGuiScale(std::get<FloatValue>(this->guiscale));
}
