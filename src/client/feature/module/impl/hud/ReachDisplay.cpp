#include "pch.h"
#include "ReachDisplay.h"
#include <sdk/common/world/level/HitResult.h>

ReachDisplay::ReachDisplay() : TextModule("ReachDisplay", "Reach Display", "Displays your reach when attacking an entity.", HUD) {
	listen<AttackEvent>((EventListenerFunc)&ReachDisplay::onAttack);

	std::get<TextValue>(this->prefix) = std::wstring(L"Reach: ");
	std::get<TextValue>(this->suffix) = std::wstring(L" blocks");

	addSliderSetting("decimals", "Decimals", "The number of decimals in the reach number", this->decimals, FloatValue(0.f), FloatValue(6.f), FloatValue(1.f));
}

std::wstringstream ReachDisplay::text(bool isDefault, bool inEditor) {
	if (std::chrono::system_clock::now() - lastAttack > 3s) {
		reach = 0.0f;
	}

	return (std::wstringstream() << std::fixed << std::setprecision((size_t)(std::get<FloatValue>(decimals)))) << reach;
}

void ReachDisplay::onAttack(Event& evG) {
	auto level = SDK::ClientInstance::get()->minecraft->getLevel();
	auto hitResult = level->getHitResult(); // the exact point the player hit

	auto hitPoint = hitResult->hitPos;
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();

	reach = lp->getPos().distance(hitPoint);
	lastAttack = std::chrono::system_clock::now();
}
