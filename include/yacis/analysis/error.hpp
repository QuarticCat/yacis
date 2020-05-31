#ifndef YACIS_ANALYSIS_ERROR_HPP_
#define YACIS_ANALYSIS_ERROR_HPP_

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "tao/pegtl.hpp"

namespace yacis::analysis {

class CompileError: public std::exception {
  public:
    using pos_t = tao::pegtl::internal::iterator;

    CompileError(pos_t pos, const std::string& error_message) noexcept:
        message(std::to_string(pos.line) + ":" +
                std::to_string(pos.byte_in_line) + " - " + error_message) {}

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
        CompileError(pos, "TypeError: " + error_message) {}
};

class DefineError: public CompileError {
  public:
    DefineError(pos_t pos, const std::string& error_message) noexcept:
        CompileError(pos, "DefineError: " + error_message) {}
};

class ParseError: public CompileError {
  public:
    ParseError(const tao::pegtl::position& pos,
               const std::string& error_message) noexcept:
        CompileError({nullptr, pos.byte, pos.line, pos.byte_in_line},
                     "ParseError: " + error_message) {}
};

}  // namespace yacis::analysis

#endif  // YACIS_ANALYSIS_ERROR_HPP_
