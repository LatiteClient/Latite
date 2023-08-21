#pragma once
#include "sdk/SDKBase.h"
#include "../TexturePtr.h"
#include "ScreenContext.h"
#include <glm/glm.hpp>

namespace SDK {
    class MinecraftUIRenderContext : public Incomplete {
    public:
        class ClientInstance* cinst;
        ScreenContext* screenContext;

        virtual ~MinecraftUIRenderContext() = 0; // 0x0
        virtual void getLineLength(DontHave<"Font&">,  std::string const&,  float,  bool) = 0; // 0x1
        virtual void getTextAlpha() = 0; // 0x2
        virtual void setTextAlpha(float) = 0; // 0x3
        virtual void drawDebugText(DontHave<"RectangleArea const&">,  std::string const&,  DontHave<"mce::Color const&">,  float,  DontHave<"ui::TextAlignment">,  DontHave<"TextMeasureData const&">, DontHave<"CaretMeasureData const&">) = 0; // 0x4
        virtual void drawText(DontHave<"Font&">, DontHave<"RectangleArea const&">, std::string const&, DontHave<"mce::Color const&">, float, DontHave<"ui::TextAlignment">, DontHave<"TextMeasureData const&">, DontHave<"CaretMeasureData const&">) = 0; // 0x5
        virtual void flushText(float) = 0; // 0x6
        virtual void drawImage(TexturePtr const&, glm::vec<2, float, (glm::qualifier)0> const&, glm::vec<2, float, (glm::qualifier)0> const&, glm::vec<2, float, (glm::qualifier)0> const&, glm::vec<2, float, (glm::qualifier)0> const&) = 0; // 0x7
        virtual void drawNineslice(TexturePtr const&, DontHave<"NinesliceInfo const&">) = 0; // 0x8
        virtual void flushImages(DontHave<"mce::Color const&">, float, DontHave<"HashedString const&">) = 0; // 0x9
        virtual void beginSharedMeshBatch(DontHave<"ComponentRenderBatch &">) = 0; // 0xA
        virtual void endSharedMeshBatch(DontHave<"ComponentRenderBatch &">) = 0; // 0xB
        virtual void drawRectangle(DontHave<"RectangleArea const&">, DontHave<"mce::Color const&">, float, int) = 0; // 0xC
        virtual void fillRectangle(DontHave<"RectangleArea const&">, DontHave<"mce::Color const&">, float) = 0; // 0xD
        virtual void increaseStencilRef() = 0; // 0xE
        virtual void decreaseStencilRef() = 0; // 0xF
        virtual void resetStencilRef() = 0; // 0x10
        virtual void fillRectangleStencil(DontHave<"RectangleArea const&">) = 0; // 0x11
        virtual void enableScissorTest(DontHave<"RectangleArea const&">) = 0; // 0x12
        virtual void disableScissorTest() = 0; // 0x13
        virtual void setClippingRectangle(DontHave<"RectangleArea const&">) = 0; // 0x14
        virtual void setFullClippingRectangle() = 0; // 0x15
        virtual void saveCurrentClippingRectangle() = 0; // 0x16
        virtual void restoreSavedClippingRectangle() = 0; // 0x17
        virtual void getFullClippingRectangle() = 0; // 0x18
        virtual void updateCustom(DontHave<"gsl::not_null<CustomRenderComponent *>">) = 0; // 0x19
        virtual void renderCustom(DontHave<"gsl::not_null<CustomRenderComponent *>">, int, DontHave<"RectangleArea const&">) = 0; // 0x1A
        virtual void cleanup() = 0; // 0x1B
        virtual void removePersistentMeshes() = 0; // 0x1C
        virtual void getTexture(DontHave<"RectangleArea const&">, bool) = 0; // 0x1D
        virtual void getZippedTexture(DontHave<"Core::Path const&">, DontHave<"RectangleArea const&">, bool) = 0; // 0x1E
        virtual void unloadTexture(DontHave<"RectangleArea const&">) = 0; // 0x1F
        virtual void getUITextureInfo(DontHave<"RectangleArea const&">, bool) = 0; // 0x20
        virtual void touchTexture(DontHave<"RectangleArea const&">) = 0; // 0x21
        virtual void getMeasureStrategy() = 0; // 0x22
        virtual void snapImageSizeToGrid(glm::vec<2, float, (glm::qualifier)0> &) = 0; // 0x23
        virtual void snapImagePositionToGrid(glm::vec<2, float, (glm::qualifier)0> &) = 0; // 0x24
        virtual void notifyImageEstimate(unsigned long) = 0; // 0x25
    };
}