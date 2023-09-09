// github.com/Imrglop/SignatureScanner
#include "PCH.h"
#include "vtable_hook.h"
#include <sstream>
#include <unordered_map>

namespace {
	// pfpTarget, pOriginal
	static std::unordered_map<LPVOID*, LPVOID*> detours;
}

int vh::hook(LPVOID* pfpTarget, LPVOID pDetour, LPVOID* pOriginal)
{
	if (pOriginal != NULL) {
		*pOriginal = *pfpTarget;
	}
	DWORD dwOldProtect;
	VirtualProtect(pfpTarget, sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*pfpTarget = pDetour;
	detours[pfpTarget] = pOriginal;
	VirtualProtect(pfpTarget, sizeof(LPVOID), dwOldProtect, &dwOldProtect);
	return 0;
}

int vh::unhook(LPVOID* pfpTarget) {
	DWORD dwOldProtect;
	VirtualProtect(pfpTarget, sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*pfpTarget = *(detours[pfpTarget]);
	VirtualProtect(pfpTarget, sizeof(LPVOID), dwOldProtect, &dwOldProtect);
	return 0;
}

void vh::unhook_all()
{
	for (auto& pair : detours) {
		unhook(pair.first);
	}
}

std::string vh::status_to_string(int status) {
	switch (status) {
	case 0:
		return "Success.";
	case 1:
		return "Function is not detoured.";
	case 2:
		return "Function already hooked.";
	default:
		return "Unknown.";
	}
}