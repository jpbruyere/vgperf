#include "VGPSceneTest.hpp"

#include "ir/scene_registry.h"

#include "ir/ir_loader.h"

#include <iostream>
#include <algorithm>

using namespace vgcpu;

const std::filesystem::path kDefaultAssetsDir = "data/scenes";
const std::filesystem::path kDefaultManifest = "data/scenes/manifest.json";

void InitSceneRegistry() {
    if (std::filesystem::exists(kDefaultManifest)) {
        auto status = vgcpu::SceneRegistry::Instance().LoadManifest(kDefaultManifest, kDefaultAssetsDir);
        if (status.failed()) {
            printf("Failed to load scene manifest: %s\n", status.message.c_str());
        }
    }
}

static bool initScenes = []() {
    InitSceneRegistry();
    auto& scene_reg = SceneRegistry::Instance();
    for (const auto& scene_id : scene_reg.GetSceneIds()) {
        auto path = scene_reg.GetScenePath(scene_id);
        if (path && std::filesystem::exists(*path)) {
            auto bytes = ir::IrLoader::LoadFromFile(*path);
            if (bytes) {
                auto result = ir::IrLoader::Prepare(*bytes, scene_id);
                if (result.ok()) {
                    std::string name = scene_id;
                    std::replace(name.begin(), name.end(), '/', '_');
                    auto ptr = std::make_unique<VGPSceneTest>(std::move(result.value()), name);
                    VGPTest::registerTest(std::move(ptr));
                }
            }
        }
    }
    return true;
}();

// Rebuild the vkvg current path from IR path data.
void VGPSceneTest::BuildPath(const Path& path) {
    ctx->NewPath();
    size_t pt_idx = 0;
    for (auto verb : path.verbs) {
        switch (verb) {
        case ir::PathVerb::kMoveTo:
            if (pt_idx + 1 <= path.points.size() / 2) {
                ctx->MoveTo(path.points[pt_idx * 2], path.points[pt_idx * 2 + 1]);
                pt_idx++;
            }
            break;
        case ir::PathVerb::kLineTo:
            if (pt_idx + 1 <= path.points.size() / 2) {
                ctx->LineTo(path.points[pt_idx * 2], path.points[pt_idx * 2 + 1]);
                pt_idx++;
            }
            break;
        case ir::PathVerb::kQuadTo:
            // vkvg doesn't have native quad bezier, convert to cubic
            if (pt_idx + 2 <= path.points.size() / 2) {
                float x0, y0;
                ctx->GetCurrentPoint(&x0, &y0);
                double x1 = path.points[pt_idx * 2];
                double y1 = path.points[pt_idx * 2 + 1];
                double x2 = path.points[(pt_idx + 1) * 2];
                double y2 = path.points[(pt_idx + 1) * 2 + 1];
                // Quad to cubic: P1 = P0 + 2/3*(C - P0), P2 = P2 + 2/3*(C - P2)
                double cx1 = x0 + (2.0 / 3.0) * (x1 - x0);
                double cy1 = y0 + (2.0 / 3.0) * (y1 - y0);
                double cx2 = x2 + (2.0 / 3.0) * (x1 - x2);
                double cy2 = y2 + (2.0 / 3.0) * (y1 - y2);
                ctx->CurveTo(cx1, cy1, cx2, cy2, x2, y2);
                pt_idx += 2;
            }
            break;
        case ir::PathVerb::kCubicTo:
            if (pt_idx + 3 <= path.points.size() / 2) {
                ctx->CurveTo(path.points[pt_idx * 2], path.points[pt_idx * 2 + 1],
                              path.points[(pt_idx + 1) * 2], path.points[(pt_idx + 1) * 2 + 1],
                              path.points[(pt_idx + 2) * 2],
                              path.points[(pt_idx + 2) * 2 + 1]);
                pt_idx += 3;
            }
            break;
        case ir::PathVerb::kClose:
            ctx->ClosePath();
            break;
        }
    }
}

void VGPSceneTest::run() {
    const uint8_t* cmd = scene.command_stream.data();
    const uint8_t* end = cmd + scene.command_stream.size();

    uint16_t current_paint_id = 0;
    ir::FillRule current_fill_rule = ir::FillRule::kNonZero;
    uint16_t current_stroke_paint_id = 0;
    float current_stroke_width = 1.0f;
    ir::StrokeCap current_stroke_cap = ir::StrokeCap::kButt;
    ir::StrokeJoin current_stroke_join = ir::StrokeJoin::kMiter;
    std::vector<float> dash_lengths;
    float dash_offset = 0.0;

    while (cmd < end) {
        ir::Opcode opcode = static_cast<ir::Opcode>(*cmd++);

        switch (opcode) {
        case ir::Opcode::kEnd:
            return;

        case ir::Opcode::kClear: {
            if (cmd + 4 > end)
                return;
            uint32_t rgba = *reinterpret_cast<const uint32_t*>(cmd);
            cmd += 4;

            float r = static_cast<float>((rgba >> 0) & 0xFF) / 255.0;
            float g = static_cast<float>((rgba >> 8) & 0xFF) / 255.0;
            float b = static_cast<float>((rgba >> 16) & 0xFF) / 255.0;
            float a = static_cast<float>((rgba >> 24) & 0xFF) / 255.0;

            ctx->Clear(r,g,b,a);
            break;
        }

        case ir::Opcode::kSetFill: {
            if (cmd + 3 > end)
                return;
            current_paint_id = *reinterpret_cast<const uint16_t*>(cmd);
            cmd += 2;
            current_fill_rule = static_cast<ir::FillRule>(*cmd++);

            const auto& paint = scene.paints[current_paint_id];

            ctx->SetPaint(paint);
            ctx->SetFillRule(current_fill_rule);
            break;
        }

        case ir::Opcode::kSetStroke: {
            if (cmd + 7 > end)
                return;
            current_stroke_paint_id = *reinterpret_cast<const uint16_t*>(cmd);
            cmd += 2;
            current_stroke_width = *reinterpret_cast<const float*>(cmd);
            cmd += 4;
            uint8_t opts = *cmd++;
            current_stroke_cap = ir::UnpackStrokeCap(opts);
            current_stroke_join = ir::UnpackStrokeJoin(opts);

            if (current_stroke_paint_id < scene.paints.size()) {
                const auto& paint = scene.paints[current_stroke_paint_id];
                ctx->SetPaint(paint);
            }
            ctx->SetLineWidth(current_stroke_width);
            ctx->SetStrokeCap(current_stroke_cap);
            ctx->SetStrokeJoin(current_stroke_join);
            break;
        }

        case ir::Opcode::kFillPath: {
            if (cmd + 2 > end)
                return;
            uint16_t path_id = *reinterpret_cast<const uint16_t*>(cmd);
            cmd += 2;

            if (path_id < scene.paths.size()) {
                BuildPath(scene.paths[path_id]);
                ctx->Fill();
            }
            break;
        }

        case ir::Opcode::kStrokePath: {
            if (cmd + 2 > end)
                return;
            uint16_t path_id = *reinterpret_cast<const uint16_t*>(cmd);
            cmd += 2;

            if (path_id < scene.paths.size()) {
                if (current_stroke_width <= 0.0f) {
                    continue;
                }
                BuildPath(scene.paths[path_id]);
                ctx->Stroke();
            }
            break;
        }

        case ir::Opcode::kSetMatrix: {
            if (cmd + 24 > end)
                return;
            const float* m = reinterpret_cast<const float*>(cmd);
            cmd += 24;
            ctx->SetMatrix (m[0], m[2], m[1], m[3], m[4], m[5]);
            break;
        }

        case ir::Opcode::kConcatMatrix: {
            if (cmd + 24 > end)
                return;
            const float* m = reinterpret_cast<const float*>(cmd);
            cmd += 24;
            ctx->ApplyTransform (m[0], m[2], m[1], m[3], m[4], m[5]);
            break;
        }

        case ir::Opcode::kSave:
            ctx->Save();
            break;

        case ir::Opcode::kRestore:
            ctx->Restore();
            break;

        case ir::Opcode::kSetDash: {
            if (cmd + 5 > end)
                return;
            uint8_t count = *cmd++;
            dash_offset = static_cast<float>(*reinterpret_cast<const float*>(cmd));
            cmd += 4;
            if (cmd + 4 * count > end)
                return;
            const float* lengths = reinterpret_cast<const float*>(cmd);
            cmd += 4 * count;
            dash_lengths.assign(lengths, lengths + count);

            ctx->SetDash(dash_lengths.empty() ? nullptr : dash_lengths.data(),
                         static_cast<int>(dash_lengths.size()), dash_offset);
            break;
        }

        case ir::Opcode::kClipPush: {
            if (cmd + 3 > end)
                return;
            uint16_t path_id = *reinterpret_cast<const uint16_t*>(cmd);
            cmd += 2;
            ir::FillRule rule = static_cast<ir::FillRule>(*cmd++);

            ctx->Save();
            if (path_id < scene.paths.size()) {
                BuildPath(scene.paths[path_id]);
                ctx->SetFillRule(rule);
                ctx->Clip();
            }
            break;
        }

        case ir::Opcode::kClipPop:
            ctx->Restore();
            break;

        default:
            return;
        }
    }
}


