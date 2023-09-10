#include "pch.h"
#include "MeshUtils.h"
#include "sdk/signature/storage.h"

void SDK::MeshHelpers::renderMeshImmediately(class ScreenContext* scn, class Tessellator* tess, class MaterialPtr* material) {
	return reinterpret_cast<decltype(&renderMeshImmediately)>(Signatures::MeshHelpers_renderMeshImmediately.result)(scn, tess, material);
}