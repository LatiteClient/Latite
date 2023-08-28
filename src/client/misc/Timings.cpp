#include "pch.h"
#include "Timings.h"

int Timings::getPerSecond(std::vector<std::chrono::steady_clock::time_point>& list)
{
	auto time = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < list.size(); i++) {
		auto start = list[i];
		auto it = std::next(list.begin(), i);
		float time_ = (time - start).count() / 1000.f / 1000.f;
		if (time_ > 1000.f) {
			list.erase(it);
		}
	}
	return static_cast<int>(list.size());
}

void Timings::update()
{
	auto dur = std::chrono::high_resolution_clock::now() - lastFPSTime;
	float dir = std::chrono::duration<float, std::milli>(dur).count();
	if (dir > 1000) {
		fps = frames;
		frames = 0;
		lastFPSTime = std::chrono::high_resolution_clock::now();
	}
	frames++;

	cpsL = getPerSecond(cpsLV);
	cpsR = getPerSecond(cpsRV);
}

void Timings::onClick(int mb, bool isDown)
{
	if (mb == 1 && isDown) {
		cpsLV.push_back(std::chrono::high_resolution_clock::now());
	}
	else if (mb == 2 && isDown) {
		cpsRV.push_back(std::chrono::high_resolution_clock::now());
	}
}
