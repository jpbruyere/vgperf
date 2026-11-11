#pragma once

#include "RenderContext.hpp"
#include "VGPerfApp.hpp"

#include "vkvg.h"

class VkvgRenderContext : public RenderContext {
    VkvgDevice   device;
    VkvgSurface  surf;
    VkvgContext  ctx;

  public:
    VkvgRenderContext() {
        this->name = "vkvg";
        //vkvg_log_level = VKVG_LOG_ERR | VKVG_LOG_WARN | VKVG_LOG_INFO;
    }

    void Init (VGPerfApp* vgapp) override {
        vkvg_device_create_info_t info {};

        device = vkvg_device_create(&info);
        surf   = vkvg_surface_create_ms(device, vgapp->width, vgapp->height, VK_SAMPLE_COUNT_8_BIT);
        //vkvg_surface_set_dpy(surf, 96, 96);
    }

    void CleanUp () override {
        vkvg_surface_destroy(surf);
        vkvg_device_destroy(device);
    }
    void InitTest () override {
        ctx = vkvg_create(surf);
    }

    void CleanUpTest () override {
        vkvg_destroy(ctx);
    }

    void Rectangle(float x, float y, float width, float height) override {
        vkvg_rectangle(ctx, x, y, width, height);
    }
    void Circle(float x, float y, float r) override {
        vkvg_arc(ctx, x, y, r, 0, (float)M_PI * 2.0f);
    }
    void Arc(float x, float y, float r, float a1, float a2) override {
        vkvg_arc(ctx, x, y, r, a1, a2);
    }

    void MoveTo(float x, float y) override {
        vkvg_move_to(ctx, x, y);
    }
    void LineTo(float x, float y) override {
        vkvg_line_to(ctx, x, y);
    }
    void CurveTo(float x1, float y1, float x2, float y2, float x3, float y3) override {
        vkvg_curve_to(ctx, x1, y1, x2, y2, x3, y3);
    }
    void Stroke() override {
        vkvg_stroke(ctx);
        //vkvg_flush(ctx);
    }
    void Fill() override {
        vkvg_fill(ctx);
        //vkvg_flush(ctx);
    }
    void Clip() override {
        vkvg_clip(ctx);
    }
    void ClosePath() override {
        vkvg_close_path(ctx);
    }
    void NewPath() override {
        vkvg_new_path(ctx);
    }

    void GetCurrentPoint(float *x, float *y) override {
        vkvg_get_current_point(ctx, x, y);
    }
    void SetSolidColor(float r, float g, float b, float a) override {
        vkvg_set_source_rgba(ctx, r, g, b, a);
    }
    void SetPaint (const Paint& paint) override {
        if (paint.type == PaintType::kSolid) {
            double r = static_cast<double>((paint.color >> 0) & 0xFF) / 255.0;
            double g = static_cast<double>((paint.color >> 8) & 0xFF) / 255.0;
            double b = static_cast<double>((paint.color >> 16) & 0xFF) / 255.0;
            double a = static_cast<double>((paint.color >> 24) & 0xFF) / 255.0;
            vkvg_set_source_rgba(ctx, r, g, b, a);
            return;
        }
        VkvgPattern pat = nullptr;
        if (paint.type == PaintType::kLinear) {
            pat = vkvg_pattern_create_linear(paint.linear_start_x, paint.linear_start_y,
                                             paint.linear_end_x, paint.linear_end_y);
        } else {
            pat = vkvg_pattern_create_radial(paint.radial_center_x, paint.radial_center_y, 0.0,
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
            vkvg_pattern_add_color_stop(pat, s.offset, r, g, b, a);
        }
        vkvg_set_source(ctx, pat);
    }
    void SetLineWidth(float w) override {
        vkvg_set_line_width(ctx, w);
    }
    void SetStrokeCap(StrokeCap strokeCap) override {
        vkvg_set_line_cap(ctx, (vkvg_line_cap_t)strokeCap);
    }
    void SetStrokeJoin(StrokeJoin strokeJoin) override {
        vkvg_set_line_join(ctx, (vkvg_line_join_t)strokeJoin);
    }
    void SetFillRule(FillRule fillRulle) override {
        vkvg_set_fill_rule(ctx, (vkvg_fill_rule_t)fillRulle);
    }
    void SetMatrix(float xx, float yx, float xy, float yy, float x0, float y0) override {
        vkvg_matrix_t mtx;
        vkvg_matrix_init(&mtx, xx, yx, xy, yy, x0, y0);
        vkvg_set_matrix(ctx, &mtx);
    }
    void ApplyTransform(float xx, float yx, float xy, float yy, float x0, float y0) override {
        vkvg_matrix_t mtx;
        vkvg_matrix_init(&mtx, xx, yx, xy, yy, x0, y0);
        vkvg_transform(ctx, &mtx);
    }
    void SetDash(const float* dashes, uint32_t num_dashes, float offset) override {
        vkvg_set_dash(ctx, dashes, num_dashes, offset);
    }
    void Save() override {
        vkvg_save(ctx);
    }
    void Restore() override {
        vkvg_restore(ctx);
    }
    void Clear(float r, float g, float b, float a) override {
        vkvg_save(ctx);
        vkvg_identity_matrix(ctx);
        vkvg_set_source_rgba(ctx, r, g, b, a);
        vkvg_set_operator(ctx, VKVG_OPERATOR_SOURCE);
        vkvg_paint(ctx);
        vkvg_restore(ctx);
        vkvg_set_operator(ctx, VKVG_OPERATOR_OVER);
    }
    void SaveImg (std::string path) override {
        vkvg_surface_write_to_png(surf, path.c_str());
    }

};
