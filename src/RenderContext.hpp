#pragma once

#include "ir/ir_format.h"

#include <string>

using namespace vgcpu::ir;

class VGPerfApp;

class RenderContext {

  protected:

  public:
    std::string name;
    virtual ~RenderContext() = default;

    virtual void Init (VGPerfApp* vgapp) = 0;
    virtual void CleanUp () = 0;
    virtual void InitTest () = 0;
    virtual void CleanUpTest () = 0;

    virtual void Rectangle(float x, float y, float width, float height) = 0;
    virtual void Circle(float x, float y, float r) = 0;
    virtual void Arc(float x, float y, float r, float a1, float a2) = 0;

    virtual void MoveTo(float x, float y) = 0;
    virtual void LineTo(float x, float y) = 0;
    virtual void CurveTo(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
    virtual void ClosePath() = 0;
    virtual void NewPath() = 0;
    virtual void GetCurrentPoint(float *x, float *y) = 0;
    virtual void Stroke() = 0;
    virtual void Fill() = 0;
    virtual void Clip() = 0;
    virtual void SetSolidColor(float r, float g, float b, float a) = 0;
    virtual void SetPaint(const Paint& paint) = 0;
    virtual void SetLineWidth(float w) = 0;
    virtual void SetStrokeCap(StrokeCap strokeCap) = 0;
    virtual void SetStrokeJoin(StrokeJoin strokeJoin) = 0;
    virtual void SetFillRule(FillRule fillRulle) = 0;
    virtual void SetMatrix(float xx, float yx, float xy, float yy, float x0, float y0) = 0;
    virtual void ApplyTransform(float xx, float yx, float xy, float yy, float x0, float y0) = 0;
    virtual void SetDash(const float* dashes, uint32_t num_dashes, float offset) = 0;

    virtual void Save() = 0;
    virtual void Restore() = 0;

    virtual void Clear(float r, float g, float b, float a) = 0;

    virtual void SaveImg (std::string path) = 0;
};
