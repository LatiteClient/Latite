#include "pch.h"
#include "Hitboxes.h"

Hitboxes::Hitboxes() : Module("Hitboxes", "Hitboxes", "Shows entity bounding boxes.", GAME) {
	addSetting("boxColor", "Box", "The hitbox color.", boxColor);
	addSetting("showEyeLine", "Show Eye Line", "Whether or not to show the eye line.", this->showEyeLine);
	addSetting("eyeLine", "Eye Line", "The eye line color.", this->eyeColor, "showEyeLine"_istrue);
	addSetting("showLookingAt", "Show Looking At", "Whether or not to show the looking at line.", this->showLine);
	addSetting("lookingAt", "Looking At", "Looking At color", this->lineColor, "showLookingAt"_istrue);
	addSetting("thirdPerson", "Show 3rd Person", "Show your own hitbox in 3rd person", this->localPlayer);
	addSetting("items", "Items", "Show item hitboxes", items);
}
