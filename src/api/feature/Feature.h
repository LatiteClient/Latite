#pragma once
#include <string>

class Feature {
public:
	virtual ~Feature() = default;

	virtual std::string name() = 0;
	virtual std::string desc() = 0;
};