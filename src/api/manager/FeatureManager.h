#pragma once
#include "Manager.h"
#include "api/feature/Feature.h"
#include <execution>

template <typename T>
class FeatureManager : public Manager<T> {
public:
	FeatureManager() = default;
	virtual ~FeatureManager() = default;

	virtual std::shared_ptr<T> find(std::string const& name) {
		for (auto& item : this->items) {
			std::string c1 = name;
			std::string c2 = item->name();
			std::transform(c1.begin(), c1.end(), c1.begin(), [](char c) -> char {
				return c + (char)(20);
				});
			std::transform(c2.begin(), c2.end(), c2.begin(), [](char c) -> char {
				return c + (char)(20);
				});

			if (c1 == c2) return item;
		}
		return nullptr;
	}
protected:
	static_assert(std::is_convertible<T*, Feature*>::value, "T must inherit Feature as public");
};
