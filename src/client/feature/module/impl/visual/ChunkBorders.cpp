#include "pch.h"
#include "ChunkBorders.h"
#include <util/DrawUtil3D.h>

static constexpr float maxY = 320.f;
static constexpr float minY = -64.f;

ChunkBorders::ChunkBorders() : Module("ChunkBorders", LocalizeString::get("client.module.chunkBorders.name"), LocalizeString::get("client.module.chunkBorders.desc"), GAME) {
	addSetting("transparent", LocalizeString::get("client.module.chunkBorders.transparent.name"), L"", transparent);

	listen<RenderLevelEvent>((EventListenerFunc)&ChunkBorders::onRender3d);
}

namespace {
	void renderFace(MCDrawUtil3D& dc, float x1, float z1, float x2, float z2, float opacity) {
		dc.drawLine({ x1, minY, z1 }, { x1, maxY, z1 }, { 0.f, 0.f, 1.f, opacity });


		float myY = SDK::ClientInstance::get()->getLocalPlayer()->getPos().y;

		float mMaxY = maxY;
		float mMinY = minY;

		for (float myY = mMinY + 2.f; myY <= mMaxY; myY += 2.f) {
			dc.drawLine({ x1, myY, z1 }, { x2, myY, z2 }, static_cast<int>(myY) % 16 == 0 ? Color(0.f, 0.f, 1.f, opacity) : Color(1.f, 1.f, 0.f, opacity));
		}

		if (!LatiteMath::aequals(x1, x2)) {
			// X mode
			// render 8 horizontal
			float myDiff = (x2 - x1) / 8.f;

			if (myDiff > 0.f) {


				for (float myX = x1 + myDiff; myX < x2; myX += myDiff) {
					dc.drawLine({ myX, minY, z1 }, { myX, maxY, z1 }, { 1.f, 1.f, 0.f, opacity });
				}
			}
			else {
				for (float myX = x1 + myDiff; myX > x2; myX += myDiff) {
					dc.drawLine({ myX, minY, z1 }, { myX, maxY, z1 }, { 1.f, 1.f, 0.f, opacity });
					dc.drawLine({ myX, minY, z1 }, { myX, maxY, z1 }, { 1.f, 1.f, 0.f, opacity });
				}
			}
		}
		else {
			float myDiff = (z2 - z1) / 8.f;

			if (myDiff > 0.f) {
				for (float myZ = z1 + myDiff; myZ < z2; myZ += myDiff) {
					dc.drawLine({ x1, minY, myZ }, { x1, maxY, myZ }, { 1.f, 1.f, 0.f, opacity });
				}
			}
			else {
				for (float myZ = z1 + myDiff; myZ > z2; myZ += myDiff) {
					dc.drawLine({ x1, minY, myZ }, { x1, maxY, myZ }, { 1.f, 1.f, 0.f, opacity });
				}
			}
			// Z mode
		}
	}

	void renderChunkBorder(MCDrawUtil3D& dc, float minX, float minZ, float opacity) {
		// last stuff


		renderFace(dc, minX, minZ, minX + 16.f, minZ, opacity);
		renderFace(dc, minX + 16.f, minZ, minX + 16.f, minZ + 16.f, opacity);
		renderFace(dc, minX + 16.f, minZ + 16.f, minX, minZ + 16.f, opacity);
		renderFace(dc, minX, minZ + 16.f, minX, minZ, opacity);


		// draw 8 horizontal ones
		for (float i = 1; i < 8; i++) {
			//dc.drawLine()
		}
	}
}


void ChunkBorders::onRender3d(Event& evG) {
	auto& ev = reinterpret_cast<RenderLevelEvent&>(evG);
	MCDrawUtil3D dc{ SDK::ClientInstance::get()->levelRenderer, ev.getScreenContext(),
		std::get<BoolValue>(this->transparent) ?
			ev.getLevelRenderer()->getLevelRendererPlayer()->getSelectionOverlayMaterial() :
			ev.getLevelRenderer()->getLevelRendererPlayer()->getSelectionBoxMaterial() };

	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) return;

	float alpha = 1.f;

	Vec3 pos = lp->getPos();
	int posX = (int)std::floor(pos.x);
	int posZ = (int)std::floor(pos.z);
	float minX = posX - static_cast<float>((posX & 15));
	float minZ = static_cast<float>(posZ - (posZ & 15));

	std::vector<std::pair<float, float>> outChunks = { {minX - 16.f, minZ - 16.f}, { minX, minZ - 16.f }, {minX + 16.f, minZ - 16.f },
		{ minX - 16.f, minZ }, { minX + 16.f, minZ },
		{ minX - 16.f, minZ + 16.f }, { minX, minZ + 16.f }, { minX + 16.f, minZ + 16.f } };

	for (auto& [x, z] : outChunks) {
		dc.drawLine({ x, minY, z }, { x, maxY, z }, d2d::Colors::RED.asAlpha(alpha));
		dc.drawLine({ x + 16.f, minY, z }, { x + 16.f, maxY, z }, d2d::Colors::RED.asAlpha(alpha));
		dc.drawLine({ x, minY, z + 16.f }, { x, maxY, z + 16.f }, d2d::Colors::RED.asAlpha(alpha));
		dc.drawLine({ x + 16.f, minY, z + 16.f }, { x + 16.f, maxY, z + 16.f }, d2d::Colors::RED.asAlpha(alpha));
	}

	renderChunkBorder(dc, minX, minZ, alpha);

	dc.flush();
}
