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
    for (const char* i = begin; i < end;) {
        const char* tmp = i;
        if (std::isdigit(*i)) {
            i += 1;
            while (i < end && isdigit(*i)) ++i;
            ret.push_back({tmp, i, TokenTag::kIntLit});
        } else if (*i == '-' && i + 1 < end && std::isdigit(*(i + 1))) {
            i += 2;
            while (i < end && isdigit(*i)) ++i;
            ret.push_back({tmp, i, TokenTag::kIntLit});
        } else if (*i == '\'') {
            i += 1;
            while (i < end && (*i != '\'' || *(i - 1) == '\\')) ++i;
            ret.push_back({tmp, i, TokenTag::kCharLit});
        } else if (std::equal(i, i + 4, "True")) {
            ret.push_back({tmp, i += 4, TokenTag::kBoolLit});
        } else if (std::equal(i, i + 5, "False")) {
            ret.push_back({tmp, i += 5, TokenTag::kBoolLit});
        } else if (isalpha(*i) || *i == '_') {
            i += 1;
            while (i < end && (isalpha(*i) || isdigit(*i) || *i == '_')) ++i;
            if (isupper(*tmp))
                ret.push_back({tmp, i, TokenTag::kUCID});
            else if (std::equal(tmp, i, "if") || std::equal(tmp, i, "then") ||
                     std::equal(tmp, i, "else") || std::equal(tmp, i, "data"))
                ret.push_back({tmp, i, TokenTag::kKeyword});
            else
                ret.push_back({tmp, i, TokenTag::kLCID});
        } else if (isspace(*i)) {
            i += 1;
            while (i < end && isspace(*i)) ++i;
            ret.push_back({tmp, i, TokenTag::kEmpty});
        } else if (*i == '-' && i + 1 < end && *(i + 1) == '-') {
            i += 2;
            while (i < end && *i != '\n') ++i;
            ret.push_back({tmp, i, TokenTag::kComment});
        } else {
            ret.push_back({tmp, ++i, TokenTag::kSymbol});
        }
    }
    return ret;
}

}  // namespace yacis::utility

#endif  // YACIS_UTILITY_TOKENIZER_HPP_
