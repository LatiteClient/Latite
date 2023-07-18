#include "ModuleManager.h"
#include "impl/TestModule.h"

ModuleManager::ModuleManager() {
	this->items.push_back(std::make_shared<TestModule>());
}
