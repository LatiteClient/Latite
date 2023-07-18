#pragma once
#include "api/eventing/EventManager.h"

class Eventing final : public IEventManager {
public:
	Eventing() = default;
	~Eventing() = default;
	//void init() override;

	// Substitute for Latite::getEventing
	[[nodiscard]] static Eventing& get();
};