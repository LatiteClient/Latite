#include "ModuleManager.h"
#include "impl/TestModule.h"
#include "impl/Zoom.h"
#include "impl/Fullbright.h"

ModuleManager::ModuleManager() {
#ifdef LATITE_DEBUG
	this->items.push_back(std::make_shared<TestModule>());
#endif
	this->items.push_back(std::make_shared<Zoom>());
	this->items.push_back(std::make_shared<Fullbright>());
}
