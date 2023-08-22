#pragma once
#include <chrono>
#include <vector>

class Timings final
{
	std::chrono::high_resolution_clock::time_point lastFPSTime{};
public:
	Timings() = default;
	Timings(Timings&) = delete;
	Timings(Timings&&) = delete;
	int getPerSecond(std::vector<std::chrono::steady_clock::time_point>& list);

	// TODO: move code from GameHooks.cpp and Latite.cpp into these
	void update();
	void onClick(int mb, bool isDown);

	[[nodiscard]] int getFPS() { return fps; }
	[[nodiscard]] int getCPSL() { return cpsL; }
	[[nodiscard]] int getCPSR() { return cpsR; }
private:

	std::chrono::steady_clock::time_point beginTime{};
	float injectTime = 0.f;

	int cpsL = 0;
	int cpsR = 0;
	int frames = 0;
	int fps = 0;
	float tps = 0;

	std::vector<std::chrono::steady_clock::time_point> cpsLV;
	std::vector<std::chrono::steady_clock::time_point> cpsRV;
	std::vector<std::chrono::steady_clock::time_point> tpsV;

};

