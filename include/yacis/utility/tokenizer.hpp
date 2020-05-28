#ifndef YACIS_UTILITY_TOKENIZER_HPP_
#define YACIS_UTILITY_TOKENIZER_HPP_

#include <cctype>
#include <cstring>
#include <string>
#include <vector>

namespace yacis::utility {

enum class TokenTag {
    kIntLit,
    kCharLit,
    kBoolLit,
    kIdent,
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
        if (isdigit(*i)) {
            const char* tmp = i++;
            while (i < end && isdigit(*i)) ++i;
            ret.push_back({tmp, i, TokenTag::kIntLit});
        } else if (*i == '-' && i + 1 < end && isdigit(*(i + 1))) {
            const char* tmp = i;
            i += 2;
            while (i < end && isdigit(*i)) ++i;
            ret.push_back({tmp, i, TokenTag::kIntLit});
        } else if (*i == '\'') {
            const char* tmp = i++;
            while (i < end && (*i != '\'' || *(i - 1) == '\\')) ++i;
            ret.push_back({tmp, i, TokenTag::kCharLit});
        } else if (*i == 'T' && !strcmp(i + 1, "rue")) {
            const char* tmp = i;
            ret.push_back({tmp, i += 4, TokenTag::kBoolLit});
        } else if (*i == 'F' && !strcmp(i + 1, "alse")) {
            const char* tmp = i;
            ret.push_back({tmp, i += 5, TokenTag::kBoolLit});
        } else if (isalpha(*i) || *i == '_') {
            const char* tmp = i++;
            while (i < end && (isalpha(*i) || *i == '_')) ++i;
            ret.push_back({tmp, i, TokenTag::kIdent});
        } else if (isspace(*i)) {
            const char* tmp = i++;
            while (i < end && isspace(*i)) ++i;
            ret.push_back({tmp, i, TokenTag::kEmpty});
        } else if (*i == '-' && i + 1 < end && *(i + 1) == '-') {
            const char* tmp = i;
            i += 2;
            while (i < end && *i != '\n') ++i;
            ret.push_back({tmp, i, TokenTag::kComment});
        } else {
            const char* tmp = i++;
            ret.push_back({tmp, i, TokenTag::kSymbol});
        }
    }
    return ret;
}

}  // namespace yacis::utility

#endif  // YACIS_UTILITY_TOKENIZER_HPP_
