#pragma once
#include "client/manager/Manager.h"
#include "client/event/Event.h"
#include "client/event/Listener.h"
#include "Module.h"
#include "script/JsModule.h"

class ModuleManager final : public Listener, public Manager<Module> {
public:
	ModuleManager();
    ~ModuleManager();

	bool registerScriptModule(JsModule* mod) {
		for (auto& mod_ : items) {
			if (mod_->name() == mod->name()) {
				return false;
			}
		}

		mod->onInit();
		this->items.push_back(std::shared_ptr<JsModule>(mod));
		JS::JsAddRef(mod->object, nullptr);
		return true;
	}

	bool deregisterScriptModule(JsModule* mod) {
		for (auto it = items.begin(); it != items.end(); it++) {
			if (it->get() == mod) {
				items.erase(it);
				return true;
			}
		}
		return false;
	}

    virtual std::shared_ptr<Module> find(std::string const& name) {
        for (auto& item : this->items) {
            std::string c1 = name;
            std::string c2 = item->name();
#pragma warning(push)
#pragma warning(disable : 4244)
            std::transform(c1.begin(), c1.end(), c1.begin(), [](unsigned char c) { return std::tolower(c); });
            std::transform(c2.begin(), c2.end(), c2.begin(), [](unsigned char c) { return std::tolower(c); });
#pragma warning(pop)
            if (c1 == c2) return item;
        }
        return nullptr;
    }

    [[nodiscard]] size_t size() const {
        return items.size();
    }

	void onKey(Event& ev);
};