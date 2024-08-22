#include "pch.h"
#include "MeshUtils.h"
#include "sdk/signature/storage.h"

void SDK::MeshHelpers::renderMeshImmediately(class ScreenContext* scn, class Tessellator* tess, class MaterialPtr* material) {
	if (internalVers >= V1_21_20) {
		char pad[0x58]{};
		using func_t = void(*)(ScreenContext*, Tessellator*, MaterialPtr*, char*);
		return reinterpret_cast<func_t>(Signatures::MeshHelpers_renderMeshImmediately.result)(scn, tess, material, pad);
	}
	else {
		return reinterpret_cast<decltype(&renderMeshImmediately)>(Signatures::MeshHelpers_renderMeshImmediately.result)(scn, tess, material);
	}
}