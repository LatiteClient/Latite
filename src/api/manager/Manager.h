#pragma once
#include <functional>
#include <memory>

template <typename T>
class Manager {
public:
	Manager() = default;
	~Manager() = default;

	void forEach(std::function<void(std::shared_ptr<T> item)> callback) {
		for (auto& it : items) {
			callback(it);
		}
	}

	void erase(std::shared_ptr<T> item) {
		for (auto it = items.begin(); it != items.end(); ++it) {
			if (*it == item) {
				items.erase(it);
				return;
			}
		}
	}

protected:
	std::vector<std::shared_ptr<T>> items = {};
};