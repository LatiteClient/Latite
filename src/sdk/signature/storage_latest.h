#pragma once
#ifdef LATITE_DEBUG
#define API_NAMES
#endif


class Signatures {
public:
	struct Offset {
		inline static SigImpl MinecraftGame_cursorGrabbed{[](memory::signature_store& store, uintptr_t) { return store.ref(2); },
			"80 b9 ? ? ? ? ? 0f 84 ? ? ? ? 48 8b 01 48 8b 80 ? ? ? ? ff 15 ? ? ? ? 48 85 c0",
			"MinecraftGame->cursorGrabbed"};

		inline static SigImpl LevelRendererPlayer_fovX{[](memory::signature_store& store, uintptr_t) { return store.ref(2); },
			"89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? f3 0f 10 0d",
			"LevelRendererPlayer->fovX"};

		//89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? 89 86 ? ? ? ? 8b 43 ? f3 0f 10 0d

		inline static SigImpl LevelRendererPlayer_origin{[](memory::signature_store& store, uintptr_t) { return store.ref(4); },
			"f3 0f 58 bb ? ? ? ? f3 0f 11 7c 24",
			"LevelRendererPlayer->origin"};

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

	// The signature is big but it hasn't died in a while soo
	inline static SigImpl Options_getGamma{[](memory::signature_store&, uintptr_t res) { return res; },
		"48 83 ec ? 80 b9 ? ? ? ? ? 48 8d 54 24 ? 48 8b 01 48 8b 40 ? 74 ? 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 f3 0f 10 42 ? 48 83 c4 ? c3 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 75 ? e8 ? ? ? ? cc e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc cc cc cc cc cc f3 0f 11 4c 24",
		"Keyboard::feed"};

	//48 83 ec ? 80 b9 ? ? ? ? ? 48 8d 54 24 ? 48 8b 01 48 8b 40 ? 74 ? 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 74 ? 48 8b 42 ? 48 8b 88 ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 83 c4 ? c3 f3 0f 10 42 ? 48 83 c4 ? c3 41 b8 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 48 85 d2 75 ? e8 ? ? ? ? cc e8 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc cc cc cc cc cc f3 0f 11 4c 24
};
