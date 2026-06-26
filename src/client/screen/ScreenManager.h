#pragma once
#include "screens/ClickGUI.h"
#include "screens/HUDEditor.h"
#include "screens/SkinStealerScreen.h"

#include "client/manager/StaticManager.h"
#include "client/event/Listener.h"
#include "client/event/events/UpdateEvent.h"
#include "Screen.h"

#include "util/Util.h"
#include "script/JsScreen.h"

class ScreenManager : public Listener, public StaticManager<Screen, ClickGUI, HUDEditor, SkinStealerScreen> {
public:
    ScreenManager();

    template<typename T>
    void showScreen(bool ignoreAnims = false) {
        auto& scr = std::get<T>(items);

        activateScreen(scr, ignoreAnims);
    }

    bool registerScriptScreen(JsScreen* jsScn) {
        bool has = false;
        forEach([&](Screen& scn) {
            if (scn.getName() == jsScn->getName()) {
                has = true;
            }
        });

        if (has) return false;

        for (auto& screen : dynamicItems) {
            if (screen->getName() == jsScn->getName()) {
                return false;
            }
        }

        this->dynamicItems.push_back(std::shared_ptr<JsScreen>(jsScn));
        JS::JsAddRef(jsScn->getObject(), nullptr);
        return true;
    }

    bool deregisterScriptScreen(JsScreen* scn) {
        return std::erase_if(dynamicItems, [&](std::shared_ptr<Screen>& obj) {
                   return obj->getName() == scn->getName();
               }) > 0;
    }

    template<typename T>
    bool tryToggleScreen() {
        auto& screen = std::get<T>(items);
        if (activeScreen && activeScreen->get().getName() == screen.getName()) {
            this->exitCurrentScreen();
            return true;
        }

        showScreen<T>();
        return true;
    }
    void exitCurrentScreen();

    [[nodiscard]] std::optional<std::reference_wrapper<Screen>> getActiveScreen() { return activeScreen; };

    void onKey(KeyUpdateEvent& ev);
    void onFocusLost(FocusLostEvent& ev);
    void onUpdate(UpdateEvent& ev);

private:
    void activateScreen(Screen& screen, bool ignoreAnims = false);
    std::optional<std::reference_wrapper<Screen>> activeScreen;
};
