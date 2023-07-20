#include "ModuleManager.h"
#include "impl/TestModule.h"
#include "impl/Zoom.h"

ModuleManager::ModuleManager() {
	this->items.push_back(std::make_shared<TestModule>());
	this->items.push_back(std::make_shared<Zoom>());
}
