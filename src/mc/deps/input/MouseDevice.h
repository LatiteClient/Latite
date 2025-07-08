#pragma once
struct MouseDevice {
	void* pad[2];
	bool mouseButtons[8];

	static MouseDevice* get();
};