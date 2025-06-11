#pragma once
#include "../../TextModule.h"

class BreakIndicator : public TextModule {
private:
    ValueType last = BoolValue(false);
    ValueType visual = BoolValue(false);
    ValueType horizontal = BoolValue(false);
    ValueType indicatorSize = FloatValue(120.f);
    ValueType indicatorWidth = FloatValue(30.f);
    ValueType indicatorCol = ColorValue({0.1f, 0.1f, 0.1f, 0.8f});
    ValueType indicatorCol2 = ColorValue({0.1f, 0.6f, 0.1f, 1.f});
    ValueType indicatorRad = FloatValue(0.f);
    ValueType padding = FloatValue(6.f);

public:
    BreakIndicator();
    ~BreakIndicator() override;

private:
    void render(DrawUtil& dc, bool isDefault, bool inEditor) override;
    std::wstringstream text(bool isDefault, bool inEditor) override;
};
