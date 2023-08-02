#include "HUDEditor.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/Eventing.h"
#include "client/Latite.h"
#include "client/config/ConfigManager.h"
#include "client/feature/module/ModuleManager.h"
#include "client/feature/module/HUDModule.h"
#include "util/DxUtil.h"
#include "util/DxContext.h"
#include "util/Util.h"
#include "../ScreenManager.h"

#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"
#include "sdk/common/client/game/MinecraftGame.h"

HUDEditor::HUDEditor() : Screen("HUDEditor") {
	Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&HUDEditor::onRender, 1, true);
}

void HUDEditor::onRender(Event& ev) {
	DXContext dc;
	if (isActive()) {
		float toBlur = Latite::get().getMenuBlur().value_or(0.f);

		auto alpha = sdk::ClientInstance::get()->minecraft->timer->alpha / 10.f;
		anim = std::lerp(anim, 1.f, alpha);

		if (Latite::get().getMenuBlur()) dc.drawGaussianBlur(toBlur * anim);

		auto& cursorPos = sdk::ClientInstance::get()->cursorPos;

		// Center Button
		{
			float buttonWidth = 200.f;
			float buttonHeight = 60.f;

			auto ss = dc.ctx->GetSize();

			d2d::Rect ssRec = { 0.f, 0.f, ss.width, ss.height };
			Vec2 btnPos = ssRec.center({ 200.f, 60.f });
			d2d::Rect btnRect = { btnPos, Vec2(btnPos.x + buttonWidth, btnPos.y + buttonHeight) };

			d2d::Color oCol = d2d::Color::RGB(0x7, 0x7, 0x7).asAlpha(0.75f);
			static d2d::Color col = oCol;
			d2d::Color outlineCol = d2d::Color::RGB(0, 0, 0).asAlpha(0.28f);

			bool state = shouldSelect(btnRect, cursorPos);
			if (state && justClicked[0]) {
				Latite::getScreenManager().exitCurrentScreen();
				Latite::getScreenManager().showScreen("ClickGUI");
				playClickSound();
			}

			col = util::LerpColorState(col, oCol + 0.1f, oCol, state);
			dc.fillRoundedRectangle(btnRect, col, 20.f);
			dc.drawRoundedRectangle(btnRect, outlineCol, 20.f, 2.f, DXContext::OutlinePosition::Outside);

			dc.drawText(btnRect, L"Mod Settings", d2d::Color(0.9f, 0.9f, 0.9f, 1.f), Renderer::FontSelection::Regular, 20.f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

		doDragging();
		doSnapping(dragOffset);
		keepModulesInBounds();
	}

	if (isActive() || sdk::ClientInstance::get()->minecraftGame->isCursorGrabbed()) {
		Latite::getModuleManager().forEach([&](std::shared_ptr<IModule> mod) {
			if (mod->isHud() && mod->isEnabled()) {
				auto hudModule = reinterpret_cast<HUDModule*>(mod.get());
				renderModule(hudModule);
			}
			});
	}
}

void HUDEditor::renderModule(HUDModule* mod) {
	DXContext dc;
	auto& cursorPos = sdk::ClientInstance::get()->cursorPos;
	bool hovering = shouldSelect(mod->getRect(), cursorPos);

	if (isActive()) {
		mod->renderFrame();
	}

	D2D1::Matrix3x2F oTrans;
	dc.ctx->GetTransform(&oTrans);
	dc.ctx->SetTransform(D2D1::Matrix3x2F::Scale(mod->getScale(), mod->getScale()) * D2D1::Matrix3x2F::Translation(mod->getRect().left, mod->getRect().top));
	mod->render(dc, false, isActive());
	dc.ctx->SetTransform(oTrans);

	if (isActive()) {
		if (hovering) {
			mod->renderSelected();
		}
	}
}


void HUDEditor::doDragging() {
	auto& cursorPos = sdk::ClientInstance::get()->cursorPos;

	bool isDown = mouseButtons[0];
	if (dragMod) {
		if (!isDown) {
			dragMod = nullptr;
		}
		else {
			dragMod->setPos(cursorPos - dragOffset);
		}
	}
	else {
		// Find a dragging element
		if (isDown) {
			bool doDrag = true;
			Latite::getModuleManager().forEach([&](std::shared_ptr<IModule> mod) {
				if (doDrag) {
					if (mod->isEnabled() && mod->isHud()) {
						HUDModule* rMod = reinterpret_cast<HUDModule*>(mod.get());
						if (shouldSelect(rMod->getRect(), cursorPos)) {
							dragMod = rMod;
							Vec2 pos = rMod->getRect().getPos();
							dragOffset = cursorPos - pos;
							doDrag = false;
						}
					}
				}
				});
				
			}
		}
}

void HUDEditor::doSnapping(Vec2 const&) {
	auto ssx = Latite::getRenderer().getDeviceContext()->GetSize();
	Vec2 ss = {ssx.width, ssx.height};
	auto mousePos = sdk::ClientInstance::get()->cursorPos;

	std::vector<float> snapLinesX = { ss.x / 4.f, ss.x / 2.f, ss.x / 2 + (ss.x / 4) };
	std::vector<float> snapLinesY = { ss.y / 2.f };

	std::vector<std::pair<float, float>> snapLinesControlsX = {};
	std::vector<float> snapLinesControlsY = {};

	// Be able to snap to the minecraft ui (like hotbar)
	/*
	for (auto& rec : controls) {
		if (rec.left > 0.f && rec.bottom < ss.y) snapLinesControlsX.emplace_back(rec.left, rec.top);
		if (rec.right > 0.f && rec.bottom < ss.y) snapLinesControlsX.emplace_back(rec.right, rec.top);
		if (rec.top > 0.f && rec.bottom < ss.y) snapLinesControlsY.push_back(rec.top);
		if (rec.bottom > 0.f && rec.bottom < ss.y) snapLinesControlsY.push_back(rec.bottom);
	}*/ // TODO: implement controls

	if (isActive() && dragMod) {
		auto pos = mousePos - dragOffset;

		float snapRange = 5.f;

		DXContext dc;

		Color col = d2d::Color(0.5, 1.0, 1.0);
		float thickness = 1.f;

		dragMod->snappingX = HUDModule::Snapping();
		dragMod->snappingY = HUDModule::Snapping();

		for (int i = 0; i < snapLinesX.size(); i++) {
			float snap = snapLinesX[i];
			SnapLine pred(dragMod, snap, false);
			pred.color = col;
			pred.thickness = thickness;

			d2d::Color col = pred.color;
			float thickness = pred.thickness;

			if (std::abs(pos.x - pred.right) < snapRange) {
				dragMod->snappingX.snap(HUDModule::Snapping::Normal, HUDModule::Snapping::Right, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ snap, 0 }, { snap, ss.y }, dc.brush, thickness);
				dragMod->setPos({ snap, dragMod->getRect().getPos().y });
			}

			if (std::abs(pos.x - pred.middle) < snapRange) {
				dragMod->snappingX.snap(HUDModule::Snapping::Normal, HUDModule::Snapping::Middle, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ snap, 0 }, { snap, ss.y }, dc.brush, thickness);
				dragMod->setPos({ pred.middle, dragMod->getRect().getPos().y });
			}

			if (std::abs(pos.x - pred.left) < snapRange) {
				dragMod->snappingX.snap(HUDModule::Snapping::Normal, HUDModule::Snapping::Left, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ snap, 0 }, { snap, ss.y }, dc.brush, thickness);
				dragMod->setPos({ pred.left, dragMod->getRect().getPos().y });
			}
		}

		for (int i = 0; i < snapLinesY.size(); i++) {
			float snap = snapLinesY[i];
			SnapLine pred(dragMod, snap, true);
			pred.color = col;
			pred.thickness = thickness;

			d2d::Color col = pred.color;
			float thickness = pred.thickness;


			if (std::abs(pos.y - pred.right) < snapRange) {
				dragMod->snappingY.snap(HUDModule::Snapping::Normal, HUDModule::Snapping::Right, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ 0.f, snap }, { ss.x, snap }, dc.brush, thickness);
				dragMod->setPos({ dragMod->getRect().getPos().x, snap });
			}

			if (std::abs(pos.y - pred.middle) < snapRange) {
				dragMod->snappingY.snap(HUDModule::Snapping::Normal, HUDModule::Snapping::Middle, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ 0.f, snap }, { ss.x, snap }, dc.brush, thickness);
				dragMod->setPos({ dragMod->getRect().getPos().x, pred.middle });
			}

			if (std::abs(pos.y - pred.left) < snapRange) {
				dragMod->snappingY.snap(HUDModule::Snapping::Normal, HUDModule::Snapping::Left, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ 0.f, snap }, { ss.x, snap }, dc.brush, thickness);
				dragMod->setPos({ dragMod->getRect().getPos().x, pred.left });
			}

		}

		for (int i = 0; i < snapLinesControlsX.size(); i++) {
			auto& snaps = snapLinesControlsX[i];
			float snap = snaps.first;
			if (std::abs(pos.y - snaps.second) > 100.f) {
				continue;
			}

			SnapLine pred(dragMod, snap, false);
			pred.color = d2d::Colors::YELLOW;
			pred.thickness = 0.5f;

			d2d::Color col = pred.color;
			float thickness = pred.thickness;

			if (std::abs(pos.x - pred.right) < 5.f) {
				dragMod->snappingX.snap(HUDModule::Snapping::MCUI, HUDModule::Snapping::Right, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ snap, 0 }, { snap, ss.y }, dc.brush, thickness);
				dragMod->setPos({ snap, dragMod->getRect().getPos().y });
			}

			if (std::abs(pos.x - pred.middle) < 5.f) {
				dragMod->snappingX.snap(HUDModule::Snapping::MCUI, HUDModule::Snapping::Middle, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ snap, 0 }, { snap, ss.y }, dc.brush, thickness);
				dragMod->setPos({ pred.middle, dragMod->getRect().getPos().y });
			}

			if (std::abs(pos.x - pred.left) < 5.f) {
				dragMod->snappingX.snap(HUDModule::Snapping::MCUI, HUDModule::Snapping::Left, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ snap, 0 }, { snap, ss.y }, dc.brush, thickness);
				dragMod->setPos({ pred.left, dragMod->getRect().getPos().y });
			}

		}

		for (int i = 0; i < snapLinesControlsY.size(); i++) {
			auto snap = snapLinesControlsY[i];
			SnapLine pred(dragMod, snap, true);
			pred.color = d2d::Colors::YELLOW;
			pred.thickness = 0.5f;

			d2d::Color col = pred.color;
			float thickness = pred.thickness;

			if (std::abs(pos.y - pred.right) < 5.f) {
				dragMod->snappingY.snap(HUDModule::Snapping::MCUI, HUDModule::Snapping::Right, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ 0.f, snap }, { ss.x, snap }, dc.brush, thickness);
				dragMod->setPos({ dragMod->getRect().getPos().x, snap });
			}

			if (std::abs(pos.y - pred.middle) < 5.f) {
				dragMod->snappingY.snap(HUDModule::Snapping::MCUI, HUDModule::Snapping::Middle, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ 0.f, snap }, { ss.x, snap }, dc.brush, thickness);
				dragMod->setPos({ dragMod->getRect().getPos().x, pred.middle });
			}

			if (std::abs(pos.y - pred.left) < 5.f) {
				dragMod->snappingY.snap(HUDModule::Snapping::MCUI, HUDModule::Snapping::Left, i);
				dc.brush->SetColor(col.get());
				dc.ctx->DrawLine({ 0.f, snap }, { ss.x, snap }, dc.brush, thickness);
				dragMod->setPos({ dragMod->getRect().getPos().x, pred.left });
			}

		}
	}
	else {
		// Keep modules in their snapped state
		Latite::getModuleManager().forEach([&](std::shared_ptr<IModule> mod) {
			if (mod->isHud()) {
				auto rMod = reinterpret_cast<HUDModule*>(mod.get());
				auto pos = rMod->getRect().getPos();
				if (rMod->snappingX.doSnapping) {
					if (rMod->snappingX.type != HUDModule::Snapping::Module) {
						auto type = rMod->snappingX.type;
						using Snapping = HUDModule::Snapping;
						auto place = rMod->snappingX.pos;
						auto idx = rMod->snappingX.idx;

						auto vector = snapLinesX;
						auto idk = snapLinesX[idx];

						// TODO: controls
						/*
						if (type == Snapping::MCUI && controls.size() > 0) {
							idk = snapLinesControlsX[idx].first;
						}*/

						SnapLine snap(rMod, idk, false);
						switch (place) {
						case Snapping::Left:
							rMod->setPos({ snap.left, pos.y });
							break;
						case Snapping::Middle:
							rMod->setPos({ snap.middle, pos.y });
							break;
						case Snapping::Right:
							rMod->setPos({ snap.right, pos.y });
							break;
						default:
							throw std::runtime_error("invalid snapping");
							break;
						}
					}
				}
				if (rMod->snappingY.doSnapping) {
					auto type = rMod->snappingY.type;
					if (rMod->snappingY.type != HUDModule::Snapping::Module) {
						using Snapping = HUDModule::Snapping;
						auto place = rMod->snappingY.pos;
						auto idx = rMod->snappingY.idx;

						auto vector = snapLinesY;
						auto idk = snapLinesY[idx];

						// TODO: controls
						/*
						if (type == Snapping::MCUI && controls.size() > 0) {
							idk = snapLinesControlsY[idx];
						}*/

						SnapLine snap(rMod, idk, true);
						switch (place) {
						case Snapping::Left:
							rMod->setPos({ pos.x, snap.left });
							break;
						case Snapping::Middle:
							rMod->setPos({ pos.x, snap.middle });
							break;
						case Snapping::Right:
							rMod->setPos({ pos.x, snap.right });
							break;
						default:
							throw std::runtime_error("invalid snapping");
							break;
						}
					}
				}

			}
			});
	}
}

void HUDEditor::keepModulesInBounds() {
	Latite::getModuleManager().forEach([&](std::shared_ptr<IModule> mod) {
		if (mod->isEnabled() && mod->isHud()) {
			HUDModule* rMod = reinterpret_cast<HUDModule*>(mod.get());
			auto ss = Latite::getRenderer().getDeviceContext()->GetSize();
			if (rMod->getRect().left < 0) {
				Vec2 modPos = rMod->getRect().getPos();
				rMod->setPos({ 0.f, modPos.y });
			}

			if (rMod->getRect().top < 0) {
				Vec2 modPos = rMod->getRect().getPos();
				rMod->setPos({ modPos.x, 0.f });
			}

			if (rMod->getRect().right > ss.width) {
				Vec2 modPos = rMod->getRect().getPos();
				rMod->setPos({ ss.width - rMod->getRect().getWidth(), modPos.y });
			}

			if (rMod->getRect().bottom > ss.height) {
				Vec2 modPos = rMod->getRect().getPos();
				rMod->setPos({ modPos.x, ss.height - rMod->getRect().getHeight() });
			}
		}
		return false;
		});
}

void HUDEditor::onEnable(bool ignoreAnims) {
	if (!ignoreAnims) anim = 0.f;
	sdk::ClientInstance::get()->releaseCursor();
}

void HUDEditor::onDisable() {
	sdk::ClientInstance::get()->grabCursor();
	Latite::getConfigManager().saveCurrentConfig();
}
