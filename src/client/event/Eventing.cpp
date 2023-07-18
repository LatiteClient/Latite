#include "Eventing.h"
#include "client/Latite.h"

Eventing& Eventing::get() {
	return Latite::getEventing();
}
