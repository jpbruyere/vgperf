// Copyright (c) 2025 Michele Fabbri (fabbri.michele@gmail.com)
// SPDX-License-Identifier: MIT

// Blueprint Reference: [ARCH-10-04] Assets & Manifest (Chapter 3) / [API-06-03] Assets/Manifest:
// scene discovery (Chapter 4)

#include "ir/scene_registry.h"

#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

namespace vgcpu {

using json = nlohmann::json;

SceneRegistry& SceneRegistry::Instance() {
    static SceneRegistry instance;
    return instance;
}

Status SceneRegistry::LoadManifest(const std::filesystem::path& manifest_path,
                                   const std::filesystem::path& assets_dir) {
    // Clear existing scenes
    Clear();
    assets_dir_ = assets_dir;

    // Open and parse manifest
    std::ifstream file(manifest_path);
    if (!file) {
        return Status::Fail("Failed to open manifest: " + manifest_path.string());
    }

    json manifest;
    try {
        file >> manifest;
    } catch (const json::exception& e) {
        return Status::Fail("Failed to parse manifest JSON: " + std::string(e.what()));
    }

    // Extract version
    manifest_version_ = manifest.value("version", "1.0.0");

    // Parse scenes array
    if (!manifest.contains("scenes") || !manifest["scenes"].is_array()) {
        return Status::Fail("Manifest missing 'scenes' array");
    }

    for (const auto& scene_json : manifest["scenes"]) {
        SceneInfo info;

        // Required fields
        if (!scene_json.contains("scene_id") || !scene_json.contains("ir_path")) {
            continue;  // Skip invalid entries
        }

        info.scene_id = scene_json["scene_id"].get<std::string>();
        info.ir_path = scene_json["ir_path"].get<std::string>();
        info.scene_hash = scene_json.value("scene_hash", "");
        info.ir_version = scene_json.value("ir_version", "1.0.0");
        info.default_width = scene_json.value("default_width", 800);
        info.default_height = scene_json.value("default_height", 600);
        info.description = scene_json.value("description", "");

        // Parse required features (using correct field names from RequiredFeatures struct)
        if (scene_json.contains("required_features")) {
            const auto& features = scene_json["required_features"];
            info.required_features.needs_nonzero = features.value("needs_nonzero", false);
            info.required_features.needs_evenodd = features.value("needs_evenodd", false);
            info.required_features.needs_cap_butt = features.value("needs_cap_butt", false);
            info.required_features.needs_cap_round = features.value("needs_cap_round", false);
            info.required_features.needs_cap_square = features.value("needs_cap_square", false);
            info.required_features.needs_join_miter = features.value("needs_join_miter", false);
            info.required_features.needs_join_round = features.value("needs_join_round", false);
            info.required_features.needs_join_bevel = features.value("needs_join_bevel", false);
            info.required_features.needs_dashes = features.value("needs_dashes", false);
            info.required_features.needs_linear_gradient =
                features.value("needs_linear_gradient", false);
            info.required_features.needs_radial_gradient =
                features.value("needs_radial_gradient", false);
            info.required_features.needs_clipping = features.value("needs_clipping", false);
        }

        // Parse tags
        if (scene_json.contains("tags") && scene_json["tags"].is_array()) {
            for (const auto& tag : scene_json["tags"]) {
                info.tags.push_back(tag.get<std::string>());
            }
        }

        scenes_.push_back(std::move(info));
    }

    return Status::Ok();
}

std::vector<std::string> SceneRegistry::GetSceneIds() const {
    std::vector<std::string> ids;
    ids.reserve(scenes_.size());
    for (const auto& scene : scenes_) {
        ids.push_back(scene.scene_id);
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

std::optional<SceneInfo> SceneRegistry::GetSceneInfo(const std::string& scene_id) const {
    for (const auto& scene : scenes_) {
        if (scene.scene_id == scene_id) {
            return scene;
        }
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> SceneRegistry::GetScenePath(
    const std::string& scene_id) const {
    for (const auto& scene : scenes_) {
        if (scene.scene_id == scene_id) {
            return assets_dir_ / scene.ir_path;
        }
    }
    return std::nullopt;
}

bool SceneRegistry::IsCompatible(const std::string& scene_id,
                                 const CapabilitySet& backend_caps) const {
    auto info = GetSceneInfo(scene_id);
    if (!info)
        return false;

    // CheckCompatibility returns empty string if compatible
    return CheckCompatibility(backend_caps, info->required_features).empty();
}

std::vector<std::string> SceneRegistry::GetCompatibleScenes(
    const CapabilitySet& backend_caps) const {
    std::vector<std::string> compatible;
    for (const auto& scene : scenes_) {
        // CheckCompatibility returns empty string if compatible
        if (CheckCompatibility(backend_caps, scene.required_features).empty()) {
            compatible.push_back(scene.scene_id);
        }
    }
    std::sort(compatible.begin(), compatible.end());
    return compatible;
}

void SceneRegistry::Clear() {
    scenes_.clear();
    manifest_version_.clear();
    assets_dir_.clear();
}

}  // namespace vgcpu
