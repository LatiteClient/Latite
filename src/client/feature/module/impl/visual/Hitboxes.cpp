#include "pch.h"
#include "Hitboxes.h"
#include <util/DrawUtil3D.h>

Hitboxes::Hitboxes() : Module("Hitboxes", LocalizeString::get("client.module.hitboxes.name"),
                              LocalizeString::get("client.module.hitboxes.desc"), GAME) {
    addSetting("transparent", LocalizeString::get("client.module.hitboxes.transparent.name"),
               LocalizeString::get("client.module.hitboxes.transparent.desc"), transparent);
    addSetting("boxColor", LocalizeString::get("client.module.hitboxes.boxColor.name"),
               LocalizeString::get("client.module.hitboxes.boxColor.desc"), boxColor);
    addSetting("showEyeLine", LocalizeString::get("client.module.hitboxes.showEyeLine.name"),
               LocalizeString::get("client.module.hitboxes.showEyeLine.desc"), this->showEyeLine);
    addSetting("eyeLine", LocalizeString::get("client.module.hitboxes.eyeLine.name"),
               LocalizeString::get("client.module.hitboxes.eyeLine.desc"), this->eyeColor, "showEyeLine"_istrue);
    addSetting("showLookingAt", LocalizeString::get("client.module.hitboxes.showLookingAt.name"),
               LocalizeString::get("client.module.hitboxes.showLookingAt.desc"), this->showLine);
    addSetting("lookingAt", LocalizeString::get("client.module.hitboxes.lookingAt.name"),
               LocalizeString::get("client.module.hitboxes.lookingAt.desc"), this->lineColor, "showLookingAt"_istrue);
    addSetting("items", LocalizeString::get("client.module.hitboxes.items.name"),
               LocalizeString::get("client.module.hitboxes.items.desc"), items);

    listen<RenderLevelEvent>(static_cast<EventListenerFunc>(&Hitboxes::onRenderLevel), false);
}

void Hitboxes::onRenderLevel(Event& evG) {
	auto& ev = reinterpret_cast<RenderLevelEvent&>(evG);
	std::vector<SDK::Actor*> actorList = SDK::ClientInstance::get()->minecraft->getLevel()->getRuntimeActorList();

	auto material = std::get<BoolValue>(transparent) ?
		SDK::MaterialPtr::getSelectionOverlayMaterial() :
		SDK::MaterialPtr::getSelectionBoxMaterial();

	auto dc = MCDrawUtil3D(SDK::ClientInstance::get()->levelRenderer, SDK::ScreenContext::instance3d, material);

	auto lp = SDK::ClientInstance::get()->getLocalPlayer();

	for (SDK::Actor* entt : actorList) {
		if (entt->isInvisible()) return;
		if (entt == lp) return;
		if (!std::get<BoolValue>(items) && entt->getEntityTypeID() == 64) return;

		Vec3 newPos = { std::lerp(entt->getPosOld().x, entt->getPos().x, SDK::ClientInstance::get()->minecraft->timer->alpha),
		std::lerp(entt->getPosOld().y, entt->getPos().y, SDK::ClientInstance::get()->minecraft->timer->alpha) ,
		std::lerp(entt->getPosOld().z, entt->getPos().z, SDK::ClientInstance::get()->minecraft->timer->alpha) };

		AABB bb = entt->getBoundingBox();
		float eyeOffset = entt->getPos().y - bb.lower.y;
		Vec3 rebasePos = newPos.operator-({ 0.f, eyeOffset, 0.f }).operator+({ 0.f, (bb.higher.y - bb.lower.y) / 2.f, 0.f });
		bb.rebase(rebasePos);

		bool willShowLine = std::get<BoolValue>(showLine) && (!entt->isPlayer() || (!SDK::RakNetConnector::get() || SDK::RakNetConnector::get()->ipAddress.empty()) || entt == lp);

		auto boxCol = std::get<ColorValue>(boxColor).getMainColor();
		auto lineCol = std::get<ColorValue>(lineColor).getMainColor();
		auto eyeCol = std::get<ColorValue>(eyeColor).getMainColor();

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
		}

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
		}
	}

	dc.flush();
}