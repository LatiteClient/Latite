#include "pch.h"
#include "ToggleSprintSneak.h"
#include "client/event/impl/TickEvent.h"
#include "client/event/impl/BeforeMoveEvent.h"
#include "client/event/impl/AfterMoveEvent.h"
#include "sdk/common/client/input/ClientInputHandler.h"

ToggleSprintSneak::ToggleSprintSneak() : TextModule("ToggleSprintSneak",
                                                    LocalizeString::get("client.textmodule.toggleSprintSneak.name"),
                                                    LocalizeString::get("client.textmodule.toggleSprintSneak.desc"),
                                                    GAMEHUD, 400.f, 0, true) {
    listen<TickEvent>(static_cast<EventListenerFunc>(&ToggleSprintSneak::onTick));
    listen<BeforeMoveEvent>(static_cast<EventListenerFunc>(&ToggleSprintSneak::beforeMove));
    listen<AfterMoveEvent>(static_cast<EventListenerFunc>(&ToggleSprintSneak::afterMove));
	listen<KeyUpdateEvent>(static_cast<EventListenerFunc>(&ToggleSprintSneak::onKey));

    addSetting("label", LocalizeString::get("client.textmodule.toggleSprintSneak.label.name"),
               LocalizeString::get("client.textmodule.toggleSprintSneak.label.desc"), label);
    addSetting("toggleSprint", LocalizeString::get("client.textmodule.toggleSprintSneak.toggleSprint.name"),
               LocalizeString::get("client.textmodule.toggleSprintSneak.toggleSprint.desc"), sprint);
    addSetting("alwaysSprint", LocalizeString::get("client.textmodule.toggleSprintSneak.alwaysSprint.name"),
               LocalizeString::get("client.textmodule.toggleSprintSneak.alwaysSprint.desc"), alwaysSprint,
               "toggleSprint"_istrue);


    //sprintMode.addEntry({ sprint_mode_normal, "Normal", "Lets the game handle the sprinting" });
    //sprintMode.addEntry({ sprint_mode_raw, "Raw", "Latite makes the player sprint" });
    //addEnumSetting("sprintMode", "Sprint Mode", "The method used by the client to sprint", sprintMode, "toggleSprint"_istrue);
    addSetting("toggleSneak", LocalizeString::get("client.textmodule.toggleSprintSneak.toggleSneak.name"),
               LocalizeString::get("client.textmodule.toggleSprintSneak.toggleSneak.desc"), sneak);
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
	else if ((std::get<BoolValue>(sprint) && toggleSprinting) || realSprint) {
		left = L"Sprinting";
		/*if (input->front && !plr->getItemUseDuration() && !input->sneak && plr->getHunger() > 6.f) { // TODO: check CollidedHorizontally/vertically + hunger
			if (!input->sprintKey) {
				//plr->setSprinting(true);
				input->sprinting = true;
			}
		}*/
		input->rawInputState.sprintDown = true;
		if (toggleSprinting) {
			right = L"Toggled";
			if (std::get<BoolValue>(alwaysSprint)) right = L"Always";
		}
		if (realSprint) right = L"Key Held";
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
	realSneaking = ev.getMoveInputHandler()->rawInputState.sneakDown;
	//realSprint = ev.getMoveInputHandler()->sprintKey;
	if (std::get<BoolValue>(sneak) && toggleSneaking) {
		ev.getMoveInputHandler()->rawInputState.sneakDown = true;
	}

	/*if (std::get<BoolValue>(sprint) && toggleSprinting) {
		ev.getMoveInputHandler()->sprintKey = true;
	}*/
}

void ToggleSprintSneak::onKey(Event& evGeneric) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);

	if (ev.getKey() == SDK::ClientInstance::get()->inputHandler->mappingFactory->defaultKeyboardLayout->findValue("sprint"))
		realSprint = ev.isDown();
}

std::wstringstream ToggleSprintSneak::text(bool isDefault, bool inEditor) {
	std::wstringstream wss;
	wss << left << " (" << right << ")";
	return wss;
}

bool ToggleSprintSneak::isActive() {
	return std::get<BoolValue>(this->label);
}
