#pragma once

#include "RenderContext.hpp"
#include "VGPerfApp.hpp"
#include "cairo/cairo.h"

class CairoRenderContext : public RenderContext {
    cairo_t*    ctx;
    cairo_surface_t* surf;

  public:
    CairoRenderContext() {
        this->name = "cairo";
    }

    void Init (VGPerfApp* vgapp) override {
        surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, vgapp->width, vgapp->height);
    }

    void CleanUp () override {
        cairo_surface_destroy(surf);
    }

    void InitTest () override {
        ctx = cairo_create(surf);
    }

    void CleanUpTest () override {
        cairo_destroy(ctx);
    }

    void Rectangle(float x, float y, float width, float height) override {
        cairo_rectangle(ctx, x, y, width, height);
    }
    void Circle(float x, float y, float r) override {
        cairo_arc(ctx, x, y, r, 0, (float)M_PI * 2.0f);
    }
    void Arc(float x, float y, float r, float a1, float a2) override {
        cairo_arc(ctx, x, y, r, a1, a2);
    }

    void MoveTo(float x, float y) override {
        cairo_move_to(ctx, x, y);
    }
    void LineTo(float x, float y) override {
        cairo_line_to(ctx, x, y);
    }
    void CurveTo(float x1, float y1, float x2, float y2, float x3, float y3) override {
        cairo_curve_to(ctx, x1, y1, x2, y2, x3, y3);
    }
    void Stroke() override {
        cairo_stroke(ctx);
    }
    void Fill() override {
        cairo_fill(ctx);
    }
    void Clip() override {
        cairo_clip(ctx);
    }
    void ClosePath() override {
        cairo_close_path(ctx);
    }
    void NewPath() override {
        cairo_new_path(ctx);
    }
    void GetCurrentPoint(float *x, float *y) override {
        double dx, dy;
        cairo_get_current_point(ctx, &dx, &dy);
        *x = (float)dx;
        *y = (float)dy;
    }
    void SetSolidColor(float r, float g, float b, float a) override {
        cairo_set_source_rgba(ctx, b, g, r, a);
    }
    void SetPaint (const Paint& paint) override {
        if (paint.type == PaintType::kSolid) {
            double r = static_cast<double>((paint.color >> 0) & 0xFF) / 255.0;
            double g = static_cast<double>((paint.color >> 8) & 0xFF) / 255.0;
            double b = static_cast<double>((paint.color >> 16) & 0xFF) / 255.0;
            double a = static_cast<double>((paint.color >> 24) & 0xFF) / 255.0;
            cairo_set_source_rgba(ctx, r, g, b, a);
            return;
        }
        cairo_pattern_t *pat = nullptr;
        if (paint.type == PaintType::kLinear) {
            pat = cairo_pattern_create_linear(paint.linear_start_x, paint.linear_start_y,
                                             paint.linear_end_x, paint.linear_end_y);
        } else {
            pat = cairo_pattern_create_radial(paint.radial_center_x, paint.radial_center_y, 0.0,
                                             paint.radial_center_x, paint.radial_center_y,
                                             paint.radial_radius);
        }
        if (paint.stops.empty())
            return;
        for (const auto& s : paint.stops) {
            double r = static_cast<double>((s.color >> 0) & 0xFF) / 255.0;
            double g = static_cast<double>((s.color >> 8) & 0xFF) / 255.0;
            double b = static_cast<double>((s.color >> 16) & 0xFF) / 255.0;
            double a = static_cast<double>((s.color >> 24) & 0xFF) / 255.0;
            cairo_pattern_add_color_stop_rgba(pat, s.offset, r, g, b, a);  // R<->B swap
        }
        cairo_set_source(ctx, pat);
    }
    void SetLineWidth(float w) override {
        cairo_set_line_width(ctx, w);
    }
    void SetStrokeCap(StrokeCap strokeCap) override {
        cairo_set_line_cap(ctx, (cairo_line_cap_t)strokeCap);
    }
    void SetStrokeJoin(StrokeJoin strokeJoin) override {
        cairo_set_line_join(ctx, (cairo_line_join_t)strokeJoin);
    }
    void SetFillRule(FillRule fillRulle) override {
        cairo_set_fill_rule(ctx, (cairo_fill_rule_t)fillRulle);
    }
    void SetMatrix(float xx, float yx, float xy, float yy, float x0, float y0) override {
        cairo_matrix_t mtx;
        cairo_matrix_init(&mtx, xx, yx, xy, yy, x0, y0);
        cairo_set_matrix(ctx, &mtx);
    }
    void ApplyTransform(float xx, float yx, float xy, float yy, float x0, float y0) override {
        cairo_matrix_t mtx;
        cairo_matrix_init(&mtx, xx, yx, xy, yy, x0, y0);
        cairo_transform(ctx, &mtx);
    }
    void SetDash(const float* dashes, uint32_t num_dashes, float offset) override {
        const double* lengths = reinterpret_cast<const double*>(dashes);
        cairo_set_dash(ctx, lengths, num_dashes, offset);
    }

    void Save() override {
        cairo_save(ctx);
    }
    void Restore() override {
        cairo_restore(ctx);
    }
    void Clear(float r, float g, float b, float a) override {
        cairo_save(ctx);
        cairo_identity_matrix(ctx);
        cairo_set_source_rgba(ctx, b, g, r, a);
        cairo_set_operator(ctx, CAIRO_OPERATOR_SOURCE);
        cairo_paint(ctx);
        cairo_restore(ctx);
        cairo_set_operator(ctx, CAIRO_OPERATOR_OVER);
    }
    void SaveImg (std::string path) override {
        cairo_surface_write_to_png(surf, path.c_str());
    }
};
