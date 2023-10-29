#include "DrawUtil3D.h"
#include "sdk/common/client/renderer/MeshUtils.h"

MCDrawUtil3D::MCDrawUtil3D(SDK::LevelRenderer* renderer, SDK::ScreenContext* ctx, SDK:MaterialPtr* material)
    : levelRenderer(renderer), screenContext(ctx), material(material) {
    
    if (!material) {
        this->material = renderer->getLevelRendererPlayer()->getSelectionBoxMaterial();
    }
}

void MCDrawUtil3D::drawLine(Vec3 const& pos1, Vec3 const& pos2, d2d::Color const& color, bool immediate) {
    auto scn = screenContext;
	auto tess = scn->tess;
	*scn->shaderColor = {1.f,1.f,1.f,1.f};
	tess->begin(4, 1); // linestrip
	tess->color(color);
	auto origin = levelRenderer->getLevelRendererPlayer()->origin;
	a = a - origin;
	b = b - origin;

	tess->vertex(a.x, a.y, a.z);
	tess->vertex(b.x, b.y, b.z);
	//renderImm(scn, tess, DrawUtils::GetClipMaterial());
    if (immediate) flush();
}

void MCDrawUtil3D::flush() {
    MeshHelpers::renderMeshImmediately(screenContext, screenContext->tess, material);
}