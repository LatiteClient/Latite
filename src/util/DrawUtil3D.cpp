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

void MCDrawUtil3D::drawQuad(Vec3 a, Vec3 b, Vec3 c, Vec3 d, d2d::Color const& col) {
	auto scn = screenContext;
	auto tess = scn->tess;
	*scn->shaderColor = { 1.f, 1.f, 1.f, 1.f };
	tess->begin(SDK::Primitive::Linestrip, 1); // linestrip
	tess->color(col);
	auto origin = levelRenderer->getLevelRendererPlayer()->getOrigin();
	a = a - (origin);
	b = b - (origin);
	c = c - (origin);
	d = d - (origin);

	tess->vertex(a.x, a.y, a.z);
	tess->vertex(b.x, b.y, b.z);


	tess->begin(SDK::Primitive::Linestrip, 1);
	tess->color(col);
	tess->vertex(b.x, b.y, b.z);
	tess->vertex(c.x, c.y, c.z);

	tess->begin(SDK::Primitive::Linestrip, 1);
	tess->color(col);
	tess->vertex(c.x, c.y, c.z);
	tess->vertex(d.x, d.y, d.z);

	tess->begin(SDK::Primitive::Linestrip, 1);
	tess->color(col);
	tess->vertex(d.x, d.y, d.z);
	tess->vertex(a.x, a.y, a.z);
}

void MCDrawUtil3D::drawBox(AABB const& bb, d2d::Color const& color) {
	auto scn = screenContext;
	auto tess = scn->tess;
	std::vector<std::pair<Vec3, Vec3>> verticies = {
		{ bb.lower, Vec3(bb.lower.x, bb.higher.y, bb.lower.z) },
		{ Vec3(bb.higher.x, bb.lower.y, bb.lower.z), Vec3(bb.higher.x, bb.higher.y, bb.lower.z)},
		{ Vec3(bb.lower.x, bb.lower.y, bb.higher.z), Vec3(bb.lower.x, bb.higher.y, bb.higher.z)},
		{ Vec3(bb.higher.x, bb.lower.y, bb.higher.z), Vec3(bb.higher.x, bb.higher.y, bb.higher.z) },
	};
	for (auto& vert : verticies) {
		drawLine(vert.first, vert.second, color);
	}

	drawQuad(Vec3(bb.lower.x, bb.lower.y, bb.lower.z), Vec3(bb.higher.x, bb.lower.y, bb.lower.z),
		Vec3(bb.higher.x, bb.lower.y, bb.higher.z), Vec3(bb.lower.x, bb.lower.y, bb.higher.z), color);

	drawQuad(Vec3(bb.lower.x, bb.higher.y, bb.lower.z), Vec3(bb.higher.x, bb.higher.y, bb.lower.z),
		Vec3(bb.higher.x, bb.higher.y, bb.higher.z), Vec3(bb.lower.x, bb.higher.y, bb.higher.z), color);

	drawQuad(Vec3(bb.lower.x, bb.higher.y, bb.lower.z), Vec3(bb.higher.x, bb.higher.y, bb.lower.z),
		Vec3(bb.higher.x, bb.higher.y, bb.higher.z), Vec3(bb.lower.x, bb.higher.y, bb.higher.z), color);
}

void MCDrawUtil3D::flush() {
    SDK::MeshHelpers::renderMeshImmediately(screenContext, screenContext->tess, material);
}