#include "pch.h"
#include "ToggleSprintSneak.h"
#include "client/event/impl/TickEvent.h"
#include "client/event/impl/BeforeMoveEvent.h"
#include "client/event/impl/AfterMoveEvent.h"

ToggleSprintSneak::ToggleSprintSneak() : TextModule("ToggleSprintSneak", "Toggle Sprint/Sneak", "Toggle sprinting or sneaking without holding the button.", GAMEHUD, 400.f, 0, true) {
	listen<TickEvent>((EventListenerFunc)&ToggleSprintSneak::onTick);
	listen<BeforeMoveEvent>((EventListenerFunc)&ToggleSprintSneak::beforeMove);
	listen<AfterMoveEvent>((EventListenerFunc)&ToggleSprintSneak::afterMove);
	
	addSetting("label", "Label", "Whether to show the label or not", label);
	addSetting("toggleSprint", "Toggle Sprint", "Toggle Sprint", sprint);
	addSetting("alwaysSprint", "Always Sprint", "Always Sprint even if sprinting is not toggled", alwaysSprint, "toggleSprint"_istrue);
	
	//sprintMode.addEntry({ sprint_mode_normal, "Normal", "Lets the game handle the sprinting" });
	//sprintMode.addEntry({ sprint_mode_raw, "Raw", "Latite makes the player sprint" });
	//addEnumSetting("sprintMode", "Sprint Mode", "The method used by the client to sprint", sprintMode, "toggleSprint"_istrue);
	addSetting("toggleSneak", "Toggle Sneak", "Toggle Sneak", sneak);
}

void ToggleSprintSneak::onTick(Event& evGeneric) {
	auto& ev = reinterpret_cast<TickEvent&>(evGeneric);
	auto plr = SDK::ClientInstance::get()->getLocalPlayer();
	if (!plr) return;
	auto input = plr->getMoveInputComponent();

	if (std::get<BoolValue>(sneak) && toggleSneaking) {
		left = L"Sneaking";
		if (toggleSneaking) {
			right = L"Toggled";
		}
	}
	else if ((std::get<BoolValue>(sprint) && toggleSprinting) || input->sprintKey) {
		left = L"Sprinting";
		if (input->front && !plr->getItemUseDuration() && !input->sneak && plr->getHunger() > 6.f) { // TODO: check CollidedHorizontally/vertically + hunger
			if (!input->sprintKey) {
				plr->setSprinting(true);
			}
		}
		if (toggleSprinting) {
			right = L"Toggled";
			if (std::get<BoolValue>(alwaysSprint)) right = L"Always";
		}
		if (input->sprintKey) right = L"Key Held";
	}
	else {
		left = L"Sprinting";
		right = L"Off";
	}

	if (std::get<BoolValue>(sprint) && std::get<BoolValue>(alwaysSprint)) {
		toggleSprinting = true;
	}

	if (realSprint && !lastSprintKey) {
		toggleSprinting = !toggleSprinting;
	}

	if (realSneaking && !lastSneakKey) {
		toggleSneaking = !toggleSneaking;
	}

	lastSprintKey = realSprint;
	lastSneakKey = realSneaking;
}

void ToggleSprintSneak::beforeMove(Event& evGeneric) {
	auto& ev = reinterpret_cast<BeforeMoveEvent&>(evGeneric);
}

void ToggleSprintSneak::afterMove(Event& evGeneric) {
	auto& ev = reinterpret_cast<AfterMoveEvent&>(evGeneric);
	realSneaking = ev.getMoveInputHandler()->sneak;
	realSprint = ev.getMoveInputHandler()->sprintKey;
	if (SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed()) {
		if (std::get<BoolValue>(sneak) && toggleSneaking) {
			ev.getMoveInputHandler()->sneak = true;
		}

		//if (std::get<BoolValue>(sprint) && toggleSprinting && sprintMode.getSelectedKey() == sprint_mode_normal) {
		//	ev.getMoveInputHandler()->sprintKey = true;
		//}
	}
}

std::wstringstream ToggleSprintSneak::text(bool isDefault, bool inEditor) {
	std::wstringstream wss;
	wss << left << " (" << right << ")";
	return wss;
}

bool ToggleSprintSneak::isActive() {
	return std::get<BoolValue>(this->label);
}
