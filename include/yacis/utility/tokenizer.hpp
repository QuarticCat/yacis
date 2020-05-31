#ifndef YACIS_UTILITY_TOKENIZER_HPP_
#define YACIS_UTILITY_TOKENIZER_HPP_

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <vector>

namespace yacis::utility {

enum class TokenTag {
    kIntLit,
    kCharLit,
    kBoolLit,
    kUCID,
    kLCID,
    kKeyword,
    kEmpty,
    kComment,
    kSymbol
};

struct Token {
    const char* begin;
    const char* end;
    TokenTag tag;
};

inline std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> ret;
    const char* begin = input.data();
    const char* end = input.data() + input.size();
    // clang-format off
    for (const char* i = begin; i < end;) {
        const char* tmp = i;
        if (std::isdigit(*i)) {
            for (++i; std::isdigit(*i); ++i);
            ret.push_back({tmp, i, TokenTag::kIntLit});
        } else if (*i == '-' && std::isdigit(*(i + 1))) {
            for (i += 2; std::isdigit(*i); ++i);
            ret.push_back({tmp, i, TokenTag::kIntLit});
        } else if (*i == '\'') {
            for (++i; *i != '\0' && (*i != '\'' || *(i - 1) == '\\'); ++i);
            ret.push_back({tmp, *i == '\'' ? ++i : i, TokenTag::kCharLit});
        } else if (std::equal(i, i + 4, "True")) {
            ret.push_back({tmp, i += 4, TokenTag::kBoolLit});
        } else if (std::equal(i, i + 5, "False")) {
            ret.push_back({tmp, i += 5, TokenTag::kBoolLit});
        } else if (std::equal(i, i + 2, "if")) {
            ret.push_back({tmp, i += 2, TokenTag::kKeyword});
        } else if (std::equal(i, i + 4, "then") ||
                   std::equal(i, i + 4, "else") ||
                   std::equal(i, i + 4, "data")) {
            ret.push_back({tmp, i += 4, TokenTag::kKeyword});
        } else if (std::isalpha(*i) || *i == '_') {
            for (++i; std::isalnum(*i) || *i == '_'; ++i);
            if (std::isupper(*tmp))
                ret.push_back({tmp, i, TokenTag::kUCID});
            else
                ret.push_back({tmp, i, TokenTag::kLCID});
        } else if (std::isspace(*i)) {
            for (++i; std::isspace(*i); ++i);
            ret.push_back({tmp, i, TokenTag::kEmpty});
        } else if (*i == '-' && i + 1 < end && *(i + 1) == '-') {
            for (i += 2; *i != '\0' && *i != '\n'; ++i);
            ret.push_back({tmp, i, TokenTag::kComment});
        } else {
            ret.push_back({tmp, ++i, TokenTag::kSymbol});
        }
    }
    // clang-format on
    return ret;
}

}  // namespace yacis::utility

#endif  // YACIS_UTILITY_TOKENIZER_HPP_
