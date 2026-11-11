// Copyright (c) 2025 Michele Fabbri (fabbri.michele@gmail.com)
// SPDX-License-Identifier: MIT

// Blueprint Reference: [ARCH-10-04] Assets & Manifest (Chapter 3) / [API-06-03] Assets/Manifest:
// scene discovery (Chapter 4)

#pragma once

#include "ir/capability_set.h"
#include "ir/status.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace vgcpu {

/// Scene metadata from manifest.json
/// Blueprint Reference: [ARCH-12-01a/b] SceneId/SceneMeta (Chapter 3) / [API-06-03] SceneMeta
/// (Chapter 4)
struct SceneInfo {
    std::string scene_id;                ///< Unique identifier (e.g., "fills/solid_basic")
    std::string ir_path;                 ///< Relative path to .irbin file
    std::string scene_hash;              ///< Content hash
    std::string ir_version;              ///< IR format version
    int32_t default_width = 800;         ///< Default render width
    int32_t default_height = 600;        ///< Default render height
    std::string description;             ///< Human-readable description
    RequiredFeatures required_features;  ///< Capability requirements
    std::vector<std::string> tags;       ///< Optional categorization tags
};

/// Scene Registry managing available benchmark scenes.
/// Blueprint Reference: [ARCH-10-04] Assets & Manifest (Chapter 3) / [ARCH-11] Allowed dependencies
/// (Chapter 3)
class SceneRegistry {
   public:
    /// Get the singleton instance.
    static SceneRegistry& Instance();

    /// Load scenes from a manifest file.
    /// @param manifest_path Path to manifest.json
    /// @param assets_dir Base directory for resolving ir_path
    Status LoadManifest(const std::filesystem::path& manifest_path,
                        const std::filesystem::path& assets_dir);

    /// Get all registered scene IDs.
    [[nodiscard]] std::vector<std::string> GetSceneIds() const;

    /// Get scene info by ID.
    [[nodiscard]] std::optional<SceneInfo> GetSceneInfo(const std::string& scene_id) const;

    /// Get the full path to a scene's IR file.
    [[nodiscard]] std::optional<std::filesystem::path> GetScenePath(
        const std::string& scene_id) const;

    /// Check if a scene is compatible with a backend's capabilities.
    [[nodiscard]] bool IsCompatible(const std::string& scene_id,
                                    const CapabilitySet& backend_caps) const;

    /// Get all scenes compatible with a backend.
    [[nodiscard]] std::vector<std::string> GetCompatibleScenes(
        const CapabilitySet& backend_caps) const;

    /// Clear all registered scenes.
    void Clear();

    /// Get the manifest version.
    [[nodiscard]] const std::string& GetManifestVersion() const { return manifest_version_; }

   private:
    SceneRegistry() = default;

    std::string manifest_version_;
    std::filesystem::path assets_dir_;
    std::vector<SceneInfo> scenes_;
};

}  // namespace vgcpu
