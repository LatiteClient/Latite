#include "DrawUtil3D.h"

void MCDrawUtil3D::drawLine(Vec3 const& pos1, Vec3 const& pos2, d2d::Color const& color) {
    auto scn = screenContext;
	auto tess = scn->tess;
	*scn->shaderColor = {1.f,1.f,1.f,1.f};
	tess->begin(4, 1); // linestrip
	tess->color(color);
	auto origin = levelRenderer->getLevelRendererPlayer()->origin;
	a = a.sub(origin);
	b = b.sub(origin);

	tess->vertex(a.x, a.y, a.z);
	tess->vertex(b.x, b.y, b.z);
	renderImm(scn, tess, DrawUtils::GetClipMaterial());
}

MCDrawUtil3D::MCDrawUtil3D(SDK::LevelRenderer* renderer, SDK::ScreenContext* ctx, SDK:MaterialPtr* material)
    : levelRenderer(renderer), screenContext(ctx) {
    
    if (!material) {
        this->material = renderer->getLevelRendererPlayer()->getSelectionBoxMaterial();
    }
}