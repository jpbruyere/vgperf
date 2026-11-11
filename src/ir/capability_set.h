// Copyright (c) 2025 Michele Fabbri (fabbri.michele@gmail.com)
// SPDX-License-Identifier: MIT

// Blueprint Reference: [ARCH-10-02] Common Types (Chapter 3)

#pragma once

#include <string>

namespace vgcpu {

/// Capability flags for backend feature support.
/// Blueprint Reference: [REQ-55] Component thread-safety (Chapter 4) / [REQ-60] Evolution rules
/// (Chapter 4)
struct CapabilitySet {
    // Fill rules
    bool supports_nonzero = true;
    bool supports_evenodd = true;

    // Stroke caps
    bool supports_cap_butt = true;
    bool supports_cap_round = true;
    bool supports_cap_square = true;

    // Stroke joins
    bool supports_join_miter = true;
    bool supports_join_round = true;
    bool supports_join_bevel = true;

    // Dash support
    bool supports_dashes = true;

    // Gradients
    bool supports_linear_gradient = true;
    bool supports_radial_gradient = true;

    // Clipping
    bool supports_clipping = true;

    // Compositing (baseline: source-over)
    bool supports_source_over = true;

    // Concurrency [REQ-35]
    bool supports_parallel_render = false;

    /// Create a CapabilitySet with all features enabled.
    static CapabilitySet All() { return {}; }

    /// Create a minimal CapabilitySet (only required baseline features).
    static CapabilitySet Minimal() {
        CapabilitySet caps;
        caps.supports_evenodd = false;
        caps.supports_cap_round = false;
        caps.supports_cap_square = false;
        caps.supports_join_round = false;
        caps.supports_join_bevel = false;
        caps.supports_dashes = false;
        caps.supports_radial_gradient = false;
        caps.supports_clipping = false;
        return caps;
    }
};

/// Required features for a scene.
/// Blueprint Reference: [ARCH-10-04] Assets & Manifest (Chapter 3) / [API-06-03] RequiredFeatures
/// (Chapter 4)
struct RequiredFeatures {
    bool needs_nonzero = false;
    bool needs_evenodd = false;
    bool needs_cap_butt = false;
    bool needs_cap_round = false;
    bool needs_cap_square = false;
    bool needs_join_miter = false;
    bool needs_join_round = false;
    bool needs_join_bevel = false;
    bool needs_dashes = false;
    bool needs_linear_gradient = false;
    bool needs_radial_gradient = false;
    bool needs_clipping = false;
};

/// Check if a backend's capabilities satisfy a scene's requirements.
/// Returns empty string if compatible, or a reason code if not.
inline std::string CheckCompatibility(const CapabilitySet& caps, const RequiredFeatures& required) {
    if (required.needs_evenodd && !caps.supports_evenodd) {
        return "UNSUPPORTED_FEATURE:evenodd";
    }
    if (required.needs_cap_round && !caps.supports_cap_round) {
        return "UNSUPPORTED_FEATURE:cap_round";
    }
    if (required.needs_cap_square && !caps.supports_cap_square) {
        return "UNSUPPORTED_FEATURE:cap_square";
    }
    if (required.needs_join_round && !caps.supports_join_round) {
        return "UNSUPPORTED_FEATURE:join_round";
    }
    if (required.needs_join_bevel && !caps.supports_join_bevel) {
        return "UNSUPPORTED_FEATURE:join_bevel";
    }
    if (required.needs_dashes && !caps.supports_dashes) {
        return "FALLBACK:dashes";
    }
    if (required.needs_clipping && !caps.supports_clipping) {
        return "FALLBACK:clipping";
    }
    return "";  // Compatible
}

}  // namespace vgcpu
