// Copyright (c) 2025 Michele Fabbri (fabbri.michele@gmail.com)
// SPDX-License-Identifier: MIT

// Blueprint Reference: [ARCH-10-05] IR Loader / Decoder (Chapter 3) / [API-06-04] IR: decoding to
// canonical representation (Chapter 4)

#pragma once

#include "ir/status.h"
#include "ir/prepared_scene.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace vgcpu {
namespace ir {

/// Validation report for IR assets.
/// Blueprint Reference: [API-03] Error handling strategy (Chapter 4) / [REQ-53] (Chapter 4)
struct ValidationReport {
    bool valid = false;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

/// IR Loader for loading and validating scene files.
/// Blueprint Reference: [ARCH-10-05] IR Loader / Decoder (Chapter 3) / [API-06-04] LoadIrFromFile
/// (Chapter 4)
class IrLoader {
   public:
    /// Load raw bytes from an IR file.
    /// @param path Path to the .irbin file.
    /// @return Optional bytes, or nullopt on I/O error.
    static std::optional<std::vector<uint8_t>> LoadFromFile(const std::filesystem::path& path);

    /// Validate IR bytes and produce a validation report.
    /// @param bytes The raw IR file bytes.
    /// @return Validation report with errors and warnings.
    static ValidationReport Validate(const std::vector<uint8_t>& bytes);

    /// Prepare a scene from validated IR bytes.
    /// @param bytes The raw IR file bytes (should be validated first).
    /// @param scene_id Optional scene ID to set on the prepared scene.
    /// @return Result containing PreparedScene or error status.
    static Result<PreparedScene> Prepare(const std::vector<uint8_t>& bytes,
                                         const std::string& scene_id = "");

    /// Compute SHA-256 hash of the IR bytes.
    /// @param bytes The raw IR file bytes.
    /// @return Lowercase hex string of the SHA-256 digest.
    static std::string ComputeHash(const std::vector<uint8_t>& bytes);

    /// Create a simple test scene for harness testing.
    /// This creates a valid PreparedScene with basic shapes.
    static PreparedScene CreateTestScene(uint32_t width = 800, uint32_t height = 600);
};

}  // namespace ir
}  // namespace vgcpu
