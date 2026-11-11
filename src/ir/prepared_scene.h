// Copyright (c) 2025 Michele Fabbri (fabbri.michele@gmail.com)
// SPDX-License-Identifier: MIT

// Blueprint Reference: [ARCH-12-01d] PreparedScene (Chapter 3) / [API-06-04] PrepareScene (Chapter
// 4)

#pragma once

#include "ir/capability_set.h"
#include "ir/ir_format.h"

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace vgcpu {

/// A single path geometry.
/// Blueprint Reference: [ARCH-10-05] Path data format (Chapter 3) / [ARCH-14-B] (Chapter 3)
struct Path {
    std::vector<ir::PathVerb> verbs;
    std::vector<float> points;  ///< x, y pairs
};


/// Immutable prepared scene optimized for replay.
/// Blueprint Reference: [ARCH-12-01d] PreparedScene (Chapter 3) / [API-06-04] PrepareScene (Chapter
/// 4)
struct PreparedScene {
    // Header info
    uint32_t width = 0;
    uint32_t height = 0;

    // Scene identification
    std::string scene_id;
    std::string scene_hash;  ///< SHA-256 hex digest
    uint8_t ir_major_version = 0;
    uint8_t ir_minor_version = 0;

    // Resource tables
    std::vector<ir::Paint> paints;
    std::vector<Path> paths;

    // Command stream (raw bytes for adapter iteration)
    std::vector<uint8_t> command_stream;

    // Feature requirements, extracted from the command stream and paint
    // table at Prepare time (IR v1.1). Drives capability gating in the
    // harness (previously a dead TODO).
    RequiredFeatures required;

    /// Check if the scene is valid and ready for rendering.
    [[nodiscard]] bool IsValid() const {
        return width > 0 && height > 0 && !command_stream.empty();
    }
};

}  // namespace vgcpu
