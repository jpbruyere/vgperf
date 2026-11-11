// Copyright (c) 2025 Michele Fabbri (fabbri.michele@gmail.com)
// SPDX-License-Identifier: MIT

// Blueprint Reference: [REQ-53] Canonical error types (Chapter 4) / [API-06-01] Common: errors and
// results (Chapter 4)

#pragma once

#include <string>
#include <variant>

namespace vgcpu {

/// Status codes for cross-module API operations.
enum class StatusCode {
    kOk,           ///< Operation succeeded
    kUnsupported,  ///< Feature or operation not supported
    kFail,         ///< Operation failed
    kInvalidArg,   ///< Invalid argument provided
    kNotFound,     ///< Resource not found
    kIOError,      ///< I/O operation failed
};

/// Structured status result for API operations.
/// Blueprint Reference: [REQ-53-02] Status/Result classes (Chapter 4)
struct Status {
    StatusCode code = StatusCode::kOk;
    std::string message;

    /// Create a success status.
    static Status Ok() { return {StatusCode::kOk, ""}; }

    /// Create an unsupported status.
    static Status Unsupported(std::string msg = "Not supported") {
        return {StatusCode::kUnsupported, std::move(msg)};
    }

    /// Create a failure status.
    static Status Fail(std::string msg) { return {StatusCode::kFail, std::move(msg)}; }

    /// Create an invalid argument status.
    static Status InvalidArg(std::string msg) { return {StatusCode::kInvalidArg, std::move(msg)}; }

    /// Create a not found status.
    static Status NotFound(std::string msg) { return {StatusCode::kNotFound, std::move(msg)}; }

    /// Create an I/O error status.
    static Status IOError(std::string msg) { return {StatusCode::kIOError, std::move(msg)}; }

    /// Check if the status indicates success.
    [[nodiscard]] bool ok() const { return code == StatusCode::kOk; }

    /// Check if the status indicates failure.
    [[nodiscard]] bool failed() const { return code != StatusCode::kOk; }
};

/// Result type that can hold either a value or an error status.
/// Blueprint Reference: [REQ-53-02] Result<T> (Chapter 4) / [API-06-01] Result template (Chapter 4)
template <typename T>
class Result {
   public:
    /// Construct a successful result with a value.
    Result(T value) : data_(std::move(value)) {}

    /// Construct a failed result with a status.
    Result(Status status) : data_(std::move(status)) {}

    /// Check if the result holds a valid value.
    [[nodiscard]] bool ok() const { return std::holds_alternative<T>(data_); }

    /// Check if the result holds an error.
    [[nodiscard]] bool failed() const { return std::holds_alternative<Status>(data_); }

    /// Get the value (undefined behavior if failed).
    [[nodiscard]] const T& value() const { return std::get<T>(data_); }
    [[nodiscard]] T& value() { return std::get<T>(data_); }

    /// Get the error status (undefined behavior if ok).
    [[nodiscard]] const Status& status() const { return std::get<Status>(data_); }

    /// Get value or a default if failed.
    [[nodiscard]] T value_or(T default_value) const {
        return ok() ? value() : std::move(default_value);
    }

   private:
    std::variant<T, Status> data_;
};

}  // namespace vgcpu
