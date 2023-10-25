#include "DxContext.h"

namespace SDK {
    class LevelRenderer;
    class ScreenContext;
}

class MCDrawUtil3D {
private:
    SDK::LevelRenderer* levelRenderer;
    SDK::ScreenContext* screenContext;
    SDK::MaterialPtr* material;
public:
    MCDrawUtil3D(SDK::LevelRenderer* renderer, SDK::ScreenContext* ctx, SDK:MaterialPtr* material = nullptr);

    void setMaterial(SDK::MaterialPtr* material) { this->material = material; }

    void drawLine(Vec3 const& pos1, Vec3 const& pos2, d2d::Color const& color, bool immediate = false);
    void flush();
}