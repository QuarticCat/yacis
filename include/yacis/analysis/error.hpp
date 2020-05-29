#ifndef YACIS_ANALYSIS_ERROR_HPP_
#define YACIS_ANALYSIS_ERROR_HPP_

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "tao/pegtl.hpp"

namespace yacis::analysis {

enum class ErrorType { kTypeError, kDefineError };

/**
 * @brief Convert given error type to its name in c style string.
 */
inline constexpr const char* error_type_to_cstr(ErrorType error_type) {
    switch (error_type) {
    case ErrorType::kTypeError:
        return "TypeError";
    case ErrorType::kDefineError:
        return "DefineError";
    default:
        return "UnknownError";
    }
}

class CompileError: public std::exception {
  public:
    using pos_t = tao::pegtl::internal::iterator;

    CompileError(ErrorType type,
                 pos_t pos,
                 const std::string& error_message) noexcept:
        message(std::to_string(pos.line) + ":" +
                std::to_string(pos.byte_in_line) + " - " +
                error_type_to_cstr(type) + ": " + error_message) {}

    CompileError(const CompileError& other) noexcept: message(other.message) {}

    CompileError& operator=(const CompileError& other) noexcept {
        message = other.message;
        return *this;
    }

    [[nodiscard]] const char* what() const noexcept override {
        return message.data();
    }

  private:
    std::string message;
};

class TypeError: public CompileError {
  public:
    TypeError(pos_t pos, const std::string& error_message) noexcept:
        CompileError(ErrorType::kTypeError, pos, error_message) {}
};

class DefineError: public CompileError {
  public:
    DefineError(pos_t pos, const std::string& error_message) noexcept:
        CompileError(ErrorType::kDefineError, pos, error_message) {}
};

}  // namespace yacis::analysis

#endif  // YACIS_ANALYSIS_ERROR_HPP_
