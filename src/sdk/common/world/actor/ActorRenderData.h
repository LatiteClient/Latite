#pragma once

namespace SDK {
    struct ActorRenderData {
        Actor* actor;
        void* extra;
        Vec3 position;
        Vec2 rotation;
        Vec2 headRot;
        bool glint;
        bool ignoreLighting;
        bool isInUI;
        float deltaTime;
        int modelObjId;
        float modelSize;
        //AnimationComponent *animationComponent;
        //MolangVariableMap *variables;

    private:
        char pad[0x10];
    };
}
