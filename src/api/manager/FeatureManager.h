#pragma once
#include "Manager.h"
#include "api/feature/Feature.h"

template <typename T>
class FeatureManager : public Manager<T> {
public:
	FeatureManager() = default;
	~FeatureManager() = default;

	virtual void init() = 0;

protected:
	static_assert(std::is_convertible<T*, Feature*>::value, "T must inherit Feature as public");
};
