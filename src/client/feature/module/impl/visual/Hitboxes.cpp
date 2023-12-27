#include "pch.h"
#include "Hitboxes.h"
#include <util/DrawUtil3D.h>

void Hitboxes::onEntityRender(Event& evG) {
	auto& ev = reinterpret_cast<AfterRenderEntityEvent&>(evG);

	auto material = std::get<BoolValue>(transparent) ?
		SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer()->getSelectionOverlayMaterial() :
		SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer()->getSelectionBoxMaterial();

	auto dc = MCDrawUtil3D(SDK::ClientInstance::get()->levelRenderer, SDK::ScreenContext::instance3d, material);

	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	auto entt = ev.getEntity();

	if (!std::get<BoolValue>(localPlayer) && entt == lp) return;
	if (!std::get<BoolValue>(items) && entt->getEntityTypeID() == 64) return;
	//if (entt->isInvisible() && (!SDK::RakNetConnector::Get() || !SDK::RakNetConnector::get()->ipAddress.empty())) return;

	Vec3 newPos = { std::lerp(entt->getPosOld().x, entt->getPos().x, SDK::ClientInstance::get()->minecraft->timer->alpha),
	std::lerp(entt->getPosOld().y, entt->getPos().y, SDK::ClientInstance::get()->minecraft->timer->alpha) ,
	std::lerp(entt->getPosOld().z, entt->getPos().z, SDK::ClientInstance::get()->minecraft->timer->alpha) };

	AABB bb = entt->getBoundingBox();
	float eyeOffset = entt->getPos().y - bb.lower.y;
	Vec3 rebasePos = newPos.operator-({ 0.f, eyeOffset, 0.f }).operator+({ 0.f, (bb.higher .y - bb.lower.y) / 2.f, 0.f });
	bb.rebase(rebasePos);

#if DEBUG
	if (mode.is(mode_hitbox)) {
		float lowY = bb.lower.y;
		bb = { bb.getCenter(), bb.getCenter() };
		bb.lower.y = lowY;
		bb.higher.y = lowY;

		auto hitbox = entt->getHitbox();

		bb.low = { bb.lower.x - hitbox.x / 2.f, lowY, bb.lower.z - hitbox.x / 2.f };
		bb.higher = { bb.higher.x + hitbox.x / 2.f, lowY + hitbox.y, bb.higher.z + hitbox.x / 2.f };
	}
#endif

	bool willShowLine = std::get<BoolValue>(showLine) && (!entt->isPlayer() || (!SDK::RakNetConnector::get() || SDK::RakNetConnector::get()->ipAddress.empty()) || entt == lp);

	//if (willShowLine || std::get<BoolValue>(showHelper)) immediate = false; // performance improvements

	auto boxCol = std::get<ColorValue>(boxColor).color1;
	auto lineCol = std::get<ColorValue>(lineColor).color1;
	auto eyeCol = std::get<ColorValue>(eyeColor).color1;


	dc.drawBox(bb, boxCol);
	float eyePos = newPos.y;
	float eyeLine = eyePos;
	bool customEyeLine = false;

	if (customEyeLine = LatiteMath::aequals(bb.lower.y, eyePos)) {
		eyeLine = bb.lower.y + (bb.higher.y - bb.lower.y) * 0.85f;
	}


	if (std::get<BoolValue>(showEyeLine)) {
		dc.drawQuad(Vec3(bb.lower.x, eyeLine, bb.lower.z), Vec3(bb.higher.x, eyeLine, bb.lower.z),
			Vec3(bb.higher.x, eyeLine, bb.higher.z), Vec3(bb.lower.x, eyeLine, bb.higher.z), eyeCol);

		if (!willShowLine) dc.flush();
	}

	//if (showHelper) {
	//	if (!willShowLine) immediate = (true);
	//
	//	float myClamp = std::clamp(lp->getPos().y, bb.lower.y, bb.higher.y);
	//
	//	dc.drawQuad(Vec3(bb.lower.x, myClamp, bb.lower.z), Vec3(bb.higher.x, myClamp, bb.lower.z),
	//		Vec3(bb.higher.x, myClamp, bb.higher.z), Vec3(bb.lower.x, myClamp, bb.higher.z), helperLineColor.getColor());
	//}

	if (willShowLine) {
		float calcYaw = (entt->getRot().y + 90) * (pi_f / 180);
		float calcPitch = entt->getRot().x * -(pi_f / 180);
		float mod = 1.f;

		Vec3 offset;
		offset.x = cos(calcYaw) * cos(calcPitch) * mod;
		offset.y = sin(calcPitch) * mod;
		offset.z = sin(calcYaw) * cos(calcPitch) * mod;

		Vec3 begin = newPos;
		begin.y = customEyeLine ? eyeLine : eyePos;
		Vec3 end = begin + offset;

		BlockPos bp{ static_cast<int>((end.x)), static_cast<int>((end.y)), static_cast<int>((end.z)) };

		dc.drawLine(begin, end, lineCol);
		dc.flush();
	}
}
Hitboxes::Hitboxes() : Module("Hitboxes", "Hitboxes", "Shows entity bounding boxes.", GAME) {
	addSetting("transparent", "Transparent", "Whether or not the hitboxes are transparent.", transparent);
	addSetting("boxColor", "Box", "The hitbox color.", boxColor);
	addSetting("showEyeLine", "Show Eye Line", "Whether or not to show the eye line.", this->showEyeLine);
	addSetting("eyeLine", "Eye Line", "The eye line color.", this->eyeColor, "showEyeLine"_istrue);
	addSetting("showLookingAt", "Show Looking At", "Whether or not to show the looking at line.", this->showLine);
	addSetting("lookingAt", "Looking At", "Looking At color", this->lineColor, "showLookingAt"_istrue);
	addSetting("thirdPerson", "Show 3rd Person", "Show your own hitbox in 3rd person", this->localPlayer);
	addSetting("items", "Items", "Show item hitboxes", items);

	listen<AfterRenderEntityEvent>((EventListenerFunc)&Hitboxes::onEntityRender, false);
}
