#pragma once
#include <vector>

class Notifications : public Listener {
public:
	Notifications();

	void push(std::wstring message) { toasts.emplace(std::move(message)); };
private:
	void onRender(Event& ev);

	struct Toast {
		std::wstring message;
		std::chrono::system_clock::time_point createTime = std::chrono::system_clock::now();
	};

	std::queue<Toast> toasts;
};