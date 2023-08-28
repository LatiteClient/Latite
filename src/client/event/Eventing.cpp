#include "Eventing.h"
#include "client/Latite.h"
#include "pch.h"

Eventing& Eventing::get() {
	return Latite::getEventing();
}
