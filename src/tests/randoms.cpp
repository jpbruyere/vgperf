#include "VGPFuncTest.hpp"

TEST(rnd_rectangles_fill) {
    for (uint32_t i = 0; i < app->testSize; i++) {
        SetRandomColor();
        DrawShape(SHAPE_RECTANGLE, 0.2);
        ctx->Fill();
    }
}
TEST(rnd_rectangles_stroke) {
    for (uint32_t i = 0; i < app->testSize; i++) {
        SetRandomColor();
        DrawShape(SHAPE_RECTANGLE, 0.2);
        ctx->Fill();
    }
}
