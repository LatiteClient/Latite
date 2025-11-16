#pragma once
#include "../TexturePtr.h"
#include "ScreenContext.h"

namespace SDK {
    namespace ui {
        // horizontal alignment, there might be a way to align vertically??
        enum class TextAlignment {
            // leading
            LEFT,
            // trailing
            RIGHT,
            CENTER,
        };
    }

    // Minecraft's RectangleArea is different from a standard rect
    struct RectangleArea {
        float left, right, top, bottom;
        // x, width, y, height
        constexpr RectangleArea(float left, float top, float right, float bottom) {
            this->left = left;
            this->right = right;
            this->top = top;
            this->bottom = bottom;
        }
    };

    struct TextMeasureData {
        float textSize = 10.f;
        float linePadding = 0.f;
        bool displayShadow = false;
        bool showColorSymbols = false;
        bool hideHyphen = false;

        constexpr TextMeasureData(float textSize, bool displayShadow, bool showColorSymbols)
            : textSize(textSize), displayShadow(displayShadow), showColorSymbols(showColorSymbols) {
        }
    };

    // A pointer to 0xFFFFFFFF by default
    struct CaretMeasureData {
        int position = -1;
        bool shouldRender = false;
    };


    class MinecraftUIRenderContext {
    public:
        class ClientInstance* cinst;
        ScreenContext* screenContext;

        void flushText(float lastFlush) {
            memory::callVirtual<void, float, std::optional<float>>(this, 6, lastFlush, {});
        }

        void drawImage(TexturePtr const& texture, Vec2 const& pos, Vec2 const& size, Vec2 const& uvPos, Vec2 const& uvSize) {
            if (internalVers >= V1_21_50) {
                memory::callVirtual<void, BedrockTextureData const&, Vec2 const&, Vec2 const&, Vec2 const&, Vec2 const&, bool>(this, 7, *texture.textureData,
                    pos, size, uvPos, uvSize, false);
            }
            else if (internalVers >= V1_21_20) {
                memory::callVirtual<void, TexturePtr const&, Vec2 const&, Vec2 const&, Vec2 const&, Vec2 const&, bool>(this, 7, texture, pos, size, uvPos, uvSize, false);
            }
            else {
                memory::callVirtual<void>(this, 7, texture, pos, size, uvPos, uvSize);
            }
        }

        virtual ~MinecraftUIRenderContext() = 0; // 0x0
        virtual void getLineLength(class Font*,  std::string const&,  float,  bool) = 0; // 0x1
        virtual float getTextAlpha() = 0; // 0x2
        virtual void setTextAlpha(float) = 0; // 0x3
        virtual void drawDebugText(RectangleArea const&,  std::string const&,  Color const&,  float,  ui::TextAlignment,  TextMeasureData const&, CaretMeasureData const&) = 0; // 0x4
        virtual void drawText(class Font*, RectangleArea const&, std::string const&, Color const&, float, ui::TextAlignment, TextMeasureData const&, CaretMeasureData const&) = 0; // 0x5
        
    private:
        virtual void flushText_(float) = 0; // 0x6
        virtual void drawImage_(TexturePtr const& texture, Vec2 const& pos, Vec2 const& size, Vec2 const& uvPos, Vec2 const& uvSize) = 0; // 0x7
    public:
        virtual void drawNineslice(TexturePtr const&, void* const&) = 0; // 0x8
        virtual void flushImages(Color const&, float, HashedString const&) = 0; // 0x9
        virtual void beginSharedMeshBatch(void*) = 0; // 0xA
        virtual void endSharedMeshBatch(void*) = 0; // 0xB
        virtual void reserveSharedMeshBatch(uint64_t) = 0; // 0xC
        virtual uint64_t getSharedMeshBatchVertexCount() = 0; // 0xD
        virtual void drawRectangle(RectangleArea const&, Color const&, float, int) = 0; // 0xE
        virtual void fillRectangle(RectangleArea const&, Color const&, float) = 0; // 0xF
        virtual void increaseStencilRef() = 0; // 0x10
        virtual void decreaseStencilRef() = 0; // 0x11
        virtual void resetStencilRef() = 0; // 0x12
        virtual void fillRectangleStencil(RectangleArea const&) = 0; // 0x13
        virtual void enableScissorTest(RectangleArea const&) = 0; // 0x14
        virtual void disableScissorTest() = 0; // 0x15
        virtual void setClippingRectangle(RectangleArea const&) = 0; // 0x16
        virtual void setFullClippingRectangle() = 0; // 0x17
        virtual void saveCurrentClippingRectangle() = 0; // 0x18
        virtual void restoreSavedClippingRectangle() = 0; // 0x19
        virtual RectangleArea getFullClippingRectangle() = 0; // 0x1A
        virtual void updateCustom(class CustomRenderComponent*) = 0; // 0x1B
        virtual void renderCustom(class CustomRenderComponent*, int, RectangleArea const&) = 0; // 0x1C
        virtual void cleanup() = 0; // 0x1D
        virtual void removePersistentMeshes() = 0; // 0x1E
        virtual TexturePtr* getTexture(TexturePtr* ptr, ResourceLocation const&, bool) = 0; // 0x1F
        virtual TexturePtr* getZippedTexture(TexturePtr* ptr, std::string const&, ResourceLocation const&, bool) = 0; // 0x20
        virtual void unloadTexture(ResourceLocation const&) = 0; // 0x21
        virtual void getUITextureInfo(ResourceLocation const&, bool) = 0; // 0x22
        virtual void touchTexture(ResourceLocation const&) = 0; // 0x23
        virtual void getMeasureStrategy() = 0; // 0x24
        virtual void snapImageSizeToGrid(Vec2 &) = 0; // 0x25
        virtual void snapImagePositionToGrid(Vec2 &) = 0; // 0x26
        virtual void notifyImageEstimate(unsigned long) = 0; // 0x27
    };
}