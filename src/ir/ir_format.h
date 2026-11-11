// Copyright (c) 2025 Michele Fabbri (fabbri.michele@gmail.com)
// SPDX-License-Identifier: MIT

// Blueprint Reference: [ARCH-10-05] IR Loader / Decoder (Chapter 3) / [API-06-04] IR: decoding to
// canonical representation (Chapter 4)

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace vgcpu {
namespace ir {

/// IR file magic bytes: 'V', 'G', 'I', 'R'
/// Blueprint Reference: [ARCH-10-05] File Header (Chapter 3) / [API-06-04] LoadIrFromFile (Chapter
/// 4)
constexpr std::array<uint8_t, 4> kIrMagic = {'V', 'G', 'I', 'R'};

/// Current IR format version.
constexpr uint8_t kIrMajorVersion = 1;
constexpr uint8_t kIrMinorVersion = 1;

/// IR File Header (16 bytes, little-endian).
/// Blueprint Reference: [ARCH-10-05] File Header (Chapter 3) / [API-06-04] Canonical IR (Chapter 4)
struct IrHeader {
    uint8_t magic[4];     ///< 'V', 'G', 'I', 'R'
    uint8_t major_ver;    ///< Major version (1)
    uint8_t minor_ver;    ///< Minor version (0)
    uint16_t reserved;    ///< Reserved (0x0000)
    uint32_t total_size;  ///< Total file size in bytes
    uint32_t scene_crc;   ///< CRC32 of scene content (excluding header)
};

static_assert(sizeof(IrHeader) == 16, "IrHeader must be exactly 16 bytes");

/// Section Type IDs.
/// Blueprint Reference: [ARCH-10-05] Section Type IDs (Chapter 3)
enum class SectionType : uint8_t {
    kInfo = 0x01,       ///< Metadata using key-value pairs
    kPaint = 0x02,      ///< Color/Gradient table
    kPath = 0x03,       ///< Path geometry table
    kCommand = 0x04,    ///< The rendering command stream
    kExtension = 0xFF,  ///< Extension section
};

/// Section Header.
/// Blueprint Reference: [ARCH-10-05] Sections (Chapter 3)
/// Note: Binary format uses 6-byte layout (type:u8, reserved:u8, length:u32).
/// In-memory representation may differ due to alignment.
struct SectionHeader {
    SectionType type;  ///< SectionTypeID
    uint8_t reserved;  ///< Reserved (0)
    uint32_t length;   ///< Section length in bytes (including this header)
};

// Binary layout constants for parsing
constexpr size_t kSectionHeaderBinarySize = 6;

/// Command Opcodes.
/// Blueprint Reference: [ARCH-14-B] IR Format Specifications (Chapter 3)
enum class Opcode : uint8_t {
    kEnd = 0x00,           ///< End of stream
    kSave = 0x01,          ///< Push state (matrix, paints)
    kRestore = 0x02,       ///< Pop state
    kClear = 0x10,         ///< Clear canvas (rgba:u32)
    kSetMatrix = 0x20,     ///< Set current transform (m:f32[6])
    kConcatMatrix = 0x21,  ///< Multiply current transform (m:f32[6])
    kSetFill = 0x30,       ///< Set fill paint & rule (paint_id:u16, rule:u8)
    kSetStroke = 0x31,     ///< Set stroke paint & params (paint_id:u16, width:f32, opts:u8)
    // IR v1.1 (2026-08-30): stroke dashing.
    // Args: count:u8, phase:f32, dashes:f32[count]. count == 0 turns
    // dashing off (solid stroke). The generator always emits an even
    // count; lengths and phase are in user units (transforms are baked
    // by the generator). Dash state applies to subsequent kStrokePath
    // commands until the next kSetDash; every Render() starts solid.
    kSetDash = 0x32,
    kFillPath = 0x40,    ///< Fill path at index (path_id:u16)
    kStrokePath = 0x41,  ///< Stroke path at index (path_id:u16)
    // IR v1.1 (2026-08-30): clipping.
    // kClipPush args: path_id:u16, rule:u8 (FillRule). Intersects the
    // current clip region with the filled path; nesting composes by
    // intersection. kClipPop removes the most recent clip. Pushes and
    // pops are balanced by the generator and are NOT affected by
    // kSave/kRestore; every Render() starts with an empty clip stack.
    kClipPush = 0x50,
    kClipPop = 0x51,
};

/// Fill rule encoding (u8 in SetFill).
/// Blueprint Reference: [ARCH-10-05] IR Loader / Decoder (Chapter 3) / [ARCH-14-B] SetFill (Chapter
/// 3)
enum class FillRule : uint8_t {
    kNonZero = 0,
    kEvenOdd = 1,
};

/// Stroke Options bitfield (u8 in SetStroke).
/// Blueprint Reference: [ARCH-10-05] IR Loader / Decoder (Chapter 3) / [ARCH-14-B] Stroke (Chapter
/// 3) Bits 0-1: Cap (0=Butt, 1=Round, 2=Square) Bits 2-3: Join (0=Miter, 1=Round, 2=Bevel) Bits
/// 4-7: Reserved
enum class StrokeCap : uint8_t {
    kButt = 0,
    kRound = 1,
    kSquare = 2,
};

enum class StrokeJoin : uint8_t {
    kMiter = 0,
    kRound = 1,
    kBevel = 2,
};

/// Utility to pack stroke options into a single byte.
inline uint8_t PackStrokeOptions(StrokeCap cap, StrokeJoin join) {
    return static_cast<uint8_t>(cap) | (static_cast<uint8_t>(join) << 2);
}

/// Utility to unpack stroke cap from options byte.
inline StrokeCap UnpackStrokeCap(uint8_t opts) {
    return static_cast<StrokeCap>(opts & 0x03);
}

/// Utility to unpack stroke join from options byte.
inline StrokeJoin UnpackStrokeJoin(uint8_t opts) {
    return static_cast<StrokeJoin>((opts >> 2) & 0x03);
}

/// Path verb codes.
/// Blueprint Reference: [ARCH-10-05] IR Loader / Decoder (Chapter 3) / [ARCH-14-B] Verb Codes
/// (Chapter 3)
enum class PathVerb : uint8_t {
    kMoveTo = 0,   ///< Move to (1 point)
    kLineTo = 1,   ///< Line to (1 point)
    kQuadTo = 2,   ///< Quadratic bezier (2 points: control, end)
    kCubicTo = 3,  ///< Cubic bezier (3 points: c1, c2, end)
    kClose = 4,    ///< Close path (0 points)
};

/// Paint type.
/// Blueprint Reference: [ARCH-10-05] IR Loader / Decoder (Chapter 3) / [ARCH-14-B] Paint (Chapter
/// 3)
enum class PaintType : uint8_t {
    kSolid = 0,
    kLinear = 1,
    kRadial = 2,
};

/// Gradient stop.
struct GradientStop {
    float offset;    ///< Position [0, 1]
    uint32_t color;  ///< RGBA8 premultiplied
};

/// A paint definition (solid color or gradient).
/// Blueprint Reference: [ARCH-10-05] Paint data format (Chapter 3) / [ARCH-14-B] (Chapter 3)
struct Paint {
    ir::PaintType type = ir::PaintType::kSolid;

           // Solid color
    uint32_t color = 0xFF000000;  ///< RGBA8 premultiplied (default: opaque black)

           // Linear gradient
    float linear_start_x = 0.0f;
    float linear_start_y = 0.0f;
    float linear_end_x = 0.0f;
    float linear_end_y = 0.0f;

           // Radial gradient
    float radial_center_x = 0.0f;
    float radial_center_y = 0.0f;
    float radial_radius = 0.0f;

           // Gradient stops (shared by linear and radial)
    std::vector<ir::GradientStop> stops;
};

}  // namespace ir
}  // namespace vgcpu
