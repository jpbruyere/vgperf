// Copyright (c) 2025 Michele Fabbri (fabbri.michele@gmail.com)
// SPDX-License-Identifier: MIT

// Blueprint Reference: [ARCH-10-05] IR Loader / Decoder (Chapter 3) / [API-06-04] IR: decoding
// (Chapter 4)

#include "ir/ir_loader.h"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace vgcpu {
namespace ir {

std::optional<std::vector<uint8_t>> IrLoader::LoadFromFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        return std::nullopt;
    }

    auto size = file.tellg();
    if (size <= 0) {
        return std::nullopt;
    }

    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return std::nullopt;
    }

    return buffer;
}

ValidationReport IrLoader::Validate(const std::vector<uint8_t>& bytes) {
    ValidationReport report;
    report.valid = true;

    if (bytes.size() < sizeof(IrHeader)) {
        report.valid = false;
        report.errors.push_back("File too small: missing IR header");
        return report;
    }

    if (bytes[0] != 'V' || bytes[1] != 'G' || bytes[2] != 'I' || bytes[3] != 'R') {
        report.valid = false;
        report.errors.push_back("Invalid magic bytes: expected 'VGIR'");
        return report;
    }

    uint8_t major = bytes[4];
    if (major != kIrMajorVersion) {
        report.valid = false;
        report.errors.push_back("Unsupported IR major version: " + std::to_string(major));
        return report;
    }

    IrHeader header;
    std::memcpy(&header, bytes.data(), sizeof(header));
    if (header.total_size != bytes.size()) {
        report.valid = false;
        report.errors.push_back("Size mismatch: header says " + std::to_string(header.total_size) +
                                " but file is " + std::to_string(bytes.size()) + " bytes");
        return report;
    }

    return report;
}

namespace {

// Helper to read little-endian values
template <typename T>
T ReadLE(const uint8_t* data) {
    T value;
    std::memcpy(&value, data, sizeof(T));
    return value;
}

// Parse Paint section
bool ParsePaintSection(const uint8_t* data, size_t len, std::vector<Paint>& paints) {
    if (len < 2)
        return false;

    uint16_t count = ReadLE<uint16_t>(data);
    data += 2;
    len -= 2;

    for (uint16_t i = 0; i < count; ++i) {
        if (len < 1)
            return false;  // type(1)

        Paint paint;
        paint.type = static_cast<PaintType>(data[0]);
        data += 1;
        len -= 1;

        if (paint.type == PaintType::kSolid) {
            if (len < 4)
                return false;
            paint.color = ReadLE<uint32_t>(data);
            data += 4;
            len -= 4;
        } else if (paint.type == PaintType::kLinear) {
            if (len < 18)
                return false;  // 4*float(16) + stop_count(2)
            paint.linear_start_x = ReadLE<float>(data);
            paint.linear_start_y = ReadLE<float>(data + 4);
            paint.linear_end_x = ReadLE<float>(data + 8);
            paint.linear_end_y = ReadLE<float>(data + 12);
            uint16_t stop_count = ReadLE<uint16_t>(data + 16);
            data += 18;
            len -= 18;

            for (uint16_t s = 0; s < stop_count; ++s) {
                if (len < 8)
                    return false;
                GradientStop stop;
                stop.offset = ReadLE<float>(data);
                stop.color = ReadLE<uint32_t>(data + 4);
                paint.stops.push_back(stop);
                data += 8;
                len -= 8;
            }
        } else if (paint.type == PaintType::kRadial) {
            if (len < 14)
                return false;  // 3*float(12) + stop_count(2)
            paint.radial_center_x = ReadLE<float>(data);
            paint.radial_center_y = ReadLE<float>(data + 4);
            paint.radial_radius = ReadLE<float>(data + 8);
            uint16_t stop_count = ReadLE<uint16_t>(data + 12);
            data += 14;
            len -= 14;

            for (uint16_t s = 0; s < stop_count; ++s) {
                if (len < 8)
                    return false;
                GradientStop stop;
                stop.offset = ReadLE<float>(data);
                stop.color = ReadLE<uint32_t>(data + 4);
                paint.stops.push_back(stop);
                data += 8;
                len -= 8;
            }
        } else {
            // Unknown paint type
            return false;
        }

        paints.push_back(std::move(paint));
    }

    return true;
}

// Parse Path section
bool ParsePathSection(const uint8_t* data, size_t len, std::vector<vgcpu::Path>& paths) {
    if (len < 2)
        return false;

    uint16_t count = ReadLE<uint16_t>(data);
    data += 2;
    len -= 2;

    for (uint16_t i = 0; i < count; ++i) {
        if (len < 8)
            return false;  // verb_count(4) + point_count(4)

        uint32_t verb_count = ReadLE<uint32_t>(data);
        uint32_t point_count = ReadLE<uint32_t>(data + 4);
        data += 8;
        len -= 8;

        vgcpu::Path path;
        path.verbs.reserve(verb_count);

        // Read verbs
        if (len < verb_count)
            return false;
        for (uint32_t v = 0; v < verb_count; ++v) {
            path.verbs.push_back(static_cast<PathVerb>(data[v]));
        }
        data += verb_count;
        len -= verb_count;

        // Read points
        size_t points_bytes = static_cast<size_t>(point_count) * sizeof(float);
        if (len < points_bytes)
            return false;
        path.points.resize(point_count);
        std::memcpy(path.points.data(), data, points_bytes);
        data += points_bytes;
        len -= points_bytes;

        paths.push_back(std::move(path));
    }

    return true;
}

}  // namespace

namespace {

// Walk the command stream and derive the scene's RequiredFeatures
// (IR v1.1). Exact by construction: requirements come from the commands
// actually present, not from manifest metadata.
void ExtractRequiredFeatures(PreparedScene& scene) {
    RequiredFeatures& req = scene.required;
    const uint8_t* cmd = scene.command_stream.data();
    const uint8_t* end = cmd + scene.command_stream.size();

    auto paint_used = [&](uint16_t paint_id) {
        if (paint_id < scene.paints.size()) {
            switch (scene.paints[paint_id].type) {
                case PaintType::kLinear:
                    req.needs_linear_gradient = true;
                    break;
                case PaintType::kRadial:
                    req.needs_radial_gradient = true;
                    break;
                default:
                    break;
            }
        }
    };

    while (cmd < end) {
        Opcode op = static_cast<Opcode>(*cmd++);
        switch (op) {
            case Opcode::kEnd:
                return;
            case Opcode::kSave:
            case Opcode::kRestore:
            case Opcode::kClipPop:
                break;
            case Opcode::kClear:
                cmd += 4;
                break;
            case Opcode::kSetMatrix:
            case Opcode::kConcatMatrix:
                cmd += 24;
                break;
            case Opcode::kSetFill: {
                if (cmd + 3 > end)
                    return;
                uint16_t paint_id = ReadLE<uint16_t>(cmd);
                if (static_cast<FillRule>(cmd[2]) == FillRule::kEvenOdd) {
                    req.needs_evenodd = true;
                } else {
                    req.needs_nonzero = true;
                }
                paint_used(paint_id);
                cmd += 3;
                break;
            }
            case Opcode::kSetStroke: {
                if (cmd + 7 > end)
                    return;
                uint16_t paint_id = ReadLE<uint16_t>(cmd);
                uint8_t opts = cmd[6];
                switch (UnpackStrokeCap(opts)) {
                    case StrokeCap::kButt:
                        req.needs_cap_butt = true;
                        break;
                    case StrokeCap::kRound:
                        req.needs_cap_round = true;
                        break;
                    case StrokeCap::kSquare:
                        req.needs_cap_square = true;
                        break;
                }
                switch (UnpackStrokeJoin(opts)) {
                    case StrokeJoin::kMiter:
                        req.needs_join_miter = true;
                        break;
                    case StrokeJoin::kRound:
                        req.needs_join_round = true;
                        break;
                    case StrokeJoin::kBevel:
                        req.needs_join_bevel = true;
                        break;
                }
                paint_used(paint_id);
                cmd += 7;
                break;
            }
            case Opcode::kSetDash: {
                if (cmd + 5 > end)
                    return;
                uint8_t count = cmd[0];
                if (count > 0) {
                    req.needs_dashes = true;
                }
                cmd += 1 + 4 + static_cast<size_t>(count) * 4;
                break;
            }
            case Opcode::kFillPath:
            case Opcode::kStrokePath:
                cmd += 2;
                break;
            case Opcode::kClipPush:
                req.needs_clipping = true;
                cmd += 3;
                break;
        }
    }
}

}  // namespace

Result<PreparedScene> IrLoader::Prepare(const std::vector<uint8_t>& bytes,
                                        const std::string& scene_id) {
    auto report = Validate(bytes);
    if (!report.valid) {
        std::string errors;
        for (const auto& e : report.errors) {
            if (!errors.empty())
                errors += "; ";
            errors += e;
        }
        return Status::Fail("IR validation failed: " + errors);
    }

    PreparedScene scene;
    scene.scene_id = scene_id;
    scene.scene_hash = ComputeHash(bytes);

    // Parse header
    IrHeader header;
    std::memcpy(&header, bytes.data(), sizeof(header));
    scene.ir_major_version = header.major_ver;
    scene.ir_minor_version = header.minor_ver;

    // Default dimensions (may be overridden by Info section)
    scene.width = 800;
    scene.height = 600;

    // Parse sections
    size_t offset = sizeof(IrHeader);
    while (offset + kSectionHeaderBinarySize <= bytes.size()) {
        // Read section header (binary format: type:u8, reserved:u8, length:u32)
        uint8_t section_type = bytes[offset];
        uint32_t section_length = ReadLE<uint32_t>(bytes.data() + offset + 2);

        if (offset + section_length > bytes.size()) {
            return Status::Fail("Section exceeds file bounds");
        }

        const uint8_t* payload = bytes.data() + offset + kSectionHeaderBinarySize;
        size_t payload_len = section_length - kSectionHeaderBinarySize;

        switch (static_cast<SectionType>(section_type)) {
            case SectionType::kPaint:
                if (!ParsePaintSection(payload, payload_len, scene.paints)) {
                    return Status::Fail("Failed to parse Paint section");
                }
                break;

            case SectionType::kPath:
                if (!ParsePathSection(payload, payload_len, scene.paths)) {
                    return Status::Fail("Failed to parse Path section");
                }
                break;

            case SectionType::kCommand:
                // Command section is stored as raw bytes for adapter interpretation
                scene.command_stream.assign(payload, payload + payload_len);
                break;

            case SectionType::kInfo:
                // TODO: Parse Info section for scene metadata
                break;

            default:
                // Skip unknown sections
                break;
        }

        offset += section_length;
    }

    if (scene.command_stream.empty()) {
        return Status::Fail("No Command section found");
    }

    ExtractRequiredFeatures(scene);

    return scene;
}

std::string IrLoader::ComputeHash(const std::vector<uint8_t>& bytes) {
    // Simple hash based on CRC32 (should be SHA-256 for production)
    // Blueprint Reference: [REQ-29] Deterministic hashing (Chapter 2) / [ARCH-12-01c] (Chapter 3)
    uint32_t crc = 0;
    for (uint8_t b : bytes) {
        crc = (crc >> 8) ^ ((crc ^ b) * 0x1EDC6F41);
    }

    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(8) << crc;
    return oss.str();
}

PreparedScene IrLoader::CreateTestScene(uint32_t width, uint32_t height) {
    PreparedScene scene;
    scene.scene_id = "test/simple_rect";
    scene.scene_hash = "test_scene_hash";
    scene.ir_major_version = kIrMajorVersion;
    scene.ir_minor_version = kIrMinorVersion;
    scene.width = width;
    scene.height = height;

    // Add a simple solid red paint
    Paint red_paint;
    red_paint.type = PaintType::kSolid;
    red_paint.color = 0xFF0000FF;  // RGBA: Red, fully opaque
    scene.paints.push_back(red_paint);

    // Add a simple rectangle path
    vgcpu::Path rect;
    rect.verbs = {PathVerb::kMoveTo, PathVerb::kLineTo, PathVerb::kLineTo, PathVerb::kLineTo,
                  PathVerb::kClose};
    rect.points = {
        100.0f, 100.0f,  // MoveTo
        300.0f, 100.0f,  // LineTo
        300.0f, 250.0f,  // LineTo
        100.0f, 250.0f,  // LineTo
    };
    scene.paths.push_back(rect);

    // Create command stream
    scene.command_stream = {static_cast<uint8_t>(Opcode::kClear),
                            0xFF,
                            0xFF,
                            0xFF,
                            0xFF,

                            static_cast<uint8_t>(Opcode::kSetFill),
                            0x00,
                            0x00,  // paint_id = 0
                            0x00,  // rule = NonZero

                            static_cast<uint8_t>(Opcode::kFillPath),
                            0x00,
                            0x00,  // path_id = 0

                            static_cast<uint8_t>(Opcode::kEnd)};

    return scene;
}

}  // namespace ir
}  // namespace vgcpu
