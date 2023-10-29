#include "pch.h"
#include "DrawUtil3D.h"
#include "sdk/common/client/renderer/MeshUtils.h"
#include <sdk/common/client/renderer/Tessellator.h>

MCDrawUtil3D::MCDrawUtil3D(SDK::LevelRenderer* renderer, SDK::ScreenContext* ctx, SDK::MaterialPtr* material)
    : levelRenderer(renderer), screenContext(ctx), material(material) {
    
    if (!material) {
        this->material = renderer->getLevelRendererPlayer()->getSelectionBoxMaterial();
    }
}

void MCDrawUtil3D::drawLine(Vec3 const& p1, Vec3 const& p2, d2d::Color const& color, bool immediate) {
    auto scn = screenContext;
	auto tess = scn->tess;
	*scn->shaderColor = {1.f,1.f,1.f,1.f};
	tess->begin(SDK::Primitive::Linestrip , 1); // linestrip
	tess->color(color);
	auto& origin = levelRenderer->getLevelRendererPlayer()->getOrigin();
	Vec3 a = p1;
	Vec3 b = p2;

	a.x -= origin.x;
	a.y -= origin.y;
	a.z -= origin.z;

	b.x -= origin.x;
	b.y -= origin.y;
	b.z -= origin.z;
	tess->vertex(a.x, a.y, a.z);
	tess->vertex(b.x, b.y, b.z);
    if (immediate) flush();
}

void MCDrawUtil3D::flush() {
    SDK::MeshHelpers::renderMeshImmediately(screenContext, screenContext->tess, material);
}