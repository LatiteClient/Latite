#pragma once
#ifdef LATITE_DEBUG
#define API_NAMES
#endif

#include "api/memory/signature/signature.h"

class SigImpl : public memory::signature_store {
public:

	explicit SigImpl(decltype(on_resolve) onResolve, std::string_view sig, std::string_view name) : signature_store("Minecraft.Windows.exe", onResolve, sig, name) {
	}
};

class Signatures {
public:
	struct Offset {
		inline static SigImpl MinecraftGame_cursorGrabbed{[](memory::signature_store& store, uintptr_t) { return store.ref(2); },
			"80 b9 ? ? ? ? ? 0f 84 ? ? ? ? 48 8b 01 48 8b 80 ? ? ? ? ff 15 ? ? ? ? 48 85 c0",
			"MinecraftGame->cursorGrabbed"};
	};
	struct Misc {
		inline static SigImpl clientInstance{[](memory::signature_store& store, uintptr_t) { return store.deref(3); },
			"48 8b 0d ? ? ? ? 48 85 c9 74 ? 48 83 39 ? 74 ? 48 8b 05 ? ? ? ? 48 85 c0 74 ? f0 ff 40 ? 48 8b 05 ? ? ? ? 48 8b 0d ? ? ? ? 48 89 43 ? 48 8b c3 48 89 3b c6 43 ? ? 48 89 4b ? 48 8b 5c 24 ? 48 83 c4 ? 5f c3 33 c0 48 8b cf 48 89 03 88 43 ? 48 89 43 ? 48 89 43 ? e8 ? ? ? ? 48 8b c3 48 8b 5c 24 ? 48 83 c4 ? 5f c3 cc cc cc cc cc cc cc cc cc cc cc e9 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc 48 89 5c 24 ? 48 89 74 24",
			"ClientInstance"};
	};

	inline static SigImpl LevelRenderer_renderLevel{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 89 5c 24 ? 55 56 57 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 49 8b f8 48 8b da 48 8b f1 0f 57 c0",
		"LevelRenderer::renderLevel"};

	inline static SigImpl Keyboard_feed{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 0f b6 c1 4c 8d 05",
		"Keyboard::feed"};
};
