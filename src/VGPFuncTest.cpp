#include "VGPFuncTest.hpp"
#include "rnd.h"

void VGPFuncTest::SetRandomColor() {
    auto rc = this->app->GetBackend();
    rc->SetSolidColor(rndf(), rndf(), rndf(), rndf());
}

const int star_points[11][2] = {{0, 85},    {75, 75},   {100, 10}, {125, 75}, {200, 85}, {150, 125},
                                {160, 190}, {100, 150}, {40, 190}, {50, 125}, {0, 85}};

void VGPFuncTest::DrawShape(shape_t shape, float sizeFact) {
    float w = (float)app->width;
    float h = (float)app->height;

    float x, y, z, v, r;

    switch (shape) {
    case SHAPE_LINE:
        x = rndf() * w;
        y = rndf() * h;
        z = rndf() * w;
        v = rndf() * h;

        ctx->MoveTo(x, y);
        ctx->LineTo(z, v);
        break;
    case SHAPE_RECTANGLE:
        z = truncf((sizeFact * w * rndf()) + 1.f);
        v = truncf((sizeFact * h * rndf()) + 1.f);
        x = truncf((w - z) * rndf());
        y = truncf((h - v) * rndf());

        ctx->Rectangle(x + 1, y + 1, z, v);
        break;
    case SHAPE_ROUNDED_RECTANGLE:
        z = truncf((sizeFact * w * rndf()) + 1.f);
        v = truncf((sizeFact * h * rndf()) + 1.f);
        x = truncf((w - z) * rndf());
        y = truncf((h - v) * rndf());
        r = truncf((0.2f * z * rndf()) + 1.f);

        if ((r > v / 2) || (r > z / 2))
            r = MIN(v / 2, z / 2);

        ctx->MoveTo(x, y + r);
        ctx->Arc(x + r, y + r, r, (float)M_PI, (float)-M_PI_2);
        ctx->LineTo(x + z - r, y);
        ctx->Arc(x + z - r, y + r, r, (float)-M_PI_2, 0);
        ctx->LineTo(x + z, y + v - r);
        ctx->Arc(x + z - r, y + v - r, r, 0, (float)M_PI_2);
        ctx->LineTo(x + r, y + v);
        ctx->Arc(x + r, y + v - r, r, (float)M_PI_2, (float)M_PI);
        ctx->LineTo(x, y + r);
        ctx->ClosePath();
        break;
    case SHAPE_CIRCLE:
        x = rndf() * w;
        y = rndf() * h;
        r = truncf((sizeFact * MIN(w, h) * rndf()) + 1.f);
        ctx->Circle(x, y, r);
        break;
    case SHAPE_TRIANGLE:
    case SHAPE_STAR:
        x = rndf() * w;
        y = rndf() * h;
        z = rndf() * sizeFact + 0.15f; // scale

        ctx->MoveTo(x + star_points[0][0] * z, y + star_points[0][1] * z);
        for (int s = 1; s < 11; s++)
            ctx->LineTo(x + star_points[s][0] * z, y + star_points[s][1] * z);
        ctx->ClosePath();
        break;
    case SHAPE_RANDOM:
        DrawShape((shape_t)(1 + (rndf() * 4)), sizeFact);
        break;
    }
}
