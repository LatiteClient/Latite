#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class PerspectiveEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(PerspectiveEvent);

	[[nodiscard]] int& getView() { return *this->view; }

	PerspectiveEvent(int& view) : view(&view) {
	}

protected:
	int* view;
};
