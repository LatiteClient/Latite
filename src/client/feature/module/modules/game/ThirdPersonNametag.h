#include "../../Module.h"

class ThirdPersonNametag : public Module {
public:
    ThirdPersonNametag() : Module("ThirdPersonNametag", LocalizeString::get("client.module.thirdPersonNametag.name"),
                                  LocalizeString::get("client.module.thirdPersonNametag.desc"), GAME) {
    }

    ~ThirdPersonNametag() override {
    }

    void onEnable() override;
    void onDisable() override;
};
