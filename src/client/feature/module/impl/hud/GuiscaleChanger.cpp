#include "pch.h"
#include "GuiscaleChanger.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/gui/GuiData.h"

GuiscaleChanger::GuiscaleChanger() : Module("GuiscaleChanger", "Guiscale Changer", "Change the scale of GUI", HUD, nokeybind) {
    this->listen<TickEvent>(&GuiscaleChanger::onTick);
    addSliderSetting("guiscale", "Guiscale", "Scale of GUI you want.", this->guiscale, FloatValue(0.5f), FloatValue(5.f), FloatValue(0.5f));
}

void GuiscaleChanger::onTick(Event& evGeneric)
{
    SDK::ClientInstance::get()->getGuiData()->updateGuiScale(std::get<FloatValue>(this->guiscale));
}
