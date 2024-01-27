#include "pch.h"
#include "ReachDisplay.h"
#include <sdk/common/world/level/HitResult.h>

ReachDisplay::ReachDisplay() : TextModule("ReachDisplay", "Reach Display", "Displays your reach.", HUD) {
	listen<AttackEvent>((EventListenerFunc)&ReachDisplay::onAttack);

	addSliderSetting("decimals", "Decimals", "The number of decimals in the reach number", this->decimals, FloatValue(0.f), FloatValue(6.f), FloatValue(1.f));
}

std::wstringstream ReachDisplay::text(bool isDefault, bool inEditor) {
	return (std::wstringstream() << std::fixed << std::setprecision((size_t)(std::get<FloatValue>(decimals)))) << reach;
}

void ReachDisplay::onAttack(Event& evG) {
	auto level = SDK::ClientInstance::get()->minecraft->getLevel();
	auto hitResult = level->getHitResult(); // the exact point the player hit

	auto hitPoint = hitResult->hitPos;
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();

	reach = lp->getPos().distance(hitPoint);
}
