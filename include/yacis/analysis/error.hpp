#ifndef YACIS_ERROR_HPP_
#define YACIS_ERROR_HPP_

#include <cstdlib>
#include <iostream>
#include <string>

#include "tao/pegtl.hpp"

namespace yacis::analysis {

using iterator_t = tao::pegtl::internal::iterator;

enum class ErrorType { kTypeError };

/**
 * @brief Convert given error type to its name in c style string.
 */
inline constexpr const char* error_type_to_cstr(ErrorType error_type) {
    switch (error_type) {
    case ErrorType::kTypeError:
        return "TypeError";
    }
}

[[noreturn]] void fatal_error(ErrorType error_type,
                              iterator_t pos,
                              const std::string& error_message) {
    std::cout << pos.line << ":" << pos.byte_in_line << " - "
              << error_type_to_cstr(error_type) << ": " << error_message;
    std::exit(EXIT_FAILURE);
}

[[noreturn]] void fatal_type_error(iterator_t pos,
                                   const std::string& error_message) {
    fatal_error(ErrorType::kTypeError, pos, error_message);
}

}  // namespace yacis::analysis

#endif  // YACIS_ERROR_HPP_
