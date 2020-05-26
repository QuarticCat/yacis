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
    char* begin;
    char* end;
    TokenTag tag;
};

inline std::vector<Token> tokenize(std::string input) {
    std::vector<Token> ret;
    char* const begin = input.data();
    char* const end = input.data() + input.size();
    for (char* i = begin; i < end;) {
        if (isdigit(*i)) {
            char* const tmp = i++;
            while (i < end && isdigit(*i)) ++i;
            ret.push_back({tmp, i, TokenTag::kIntLit});
        } else if (*i == '-' && i + 1 < end && isdigit(*(i + 1))) {
            char* const tmp = i;
            i += 2;
            while (i < end && isdigit(*i)) ++i;
            ret.push_back({tmp, i, TokenTag::kIntLit});
        } else if (*i == '\'') {
            char* const tmp = i++;
            while (i < end && (*i != '\'' || *(i - 1) == '\\')) ++i;
            ret.push_back({tmp, i, TokenTag::kCharLit});
        } else if (*i == 'T' && !strcmp(i + 1, "rue")) {
            char* const tmp = i;
            ret.push_back({tmp, i += 4, TokenTag::kBoolLit});
        } else if (*i == 'F' && !strcmp(i + 1, "alse")) {
            char* const tmp = i;
            ret.push_back({tmp, i += 5, TokenTag::kBoolLit});
        } else if (isalpha(*i) || *i == '_') {
            char* const tmp = i++;
            while (i < end && (isalpha(*i) || *i == '_')) ++i;
            ret.push_back({tmp, i, TokenTag::kIdent});
        } else if (isspace(*i)) {
            char* const tmp = i++;
            while (i < end && isspace(*i)) ++i;
            ret.push_back({tmp, i, TokenTag::kEmpty});
        } else if (*i == '-' && i + 1 < end && *(i + 1) == '-') {
            char* const tmp = i;
            i += 2;
            while (i < end && *i != '\n') ++i;
            ret.push_back({tmp, i, TokenTag::kComment});
        } else {
            char* const tmp = i++;
            ret.push_back({tmp, i, TokenTag::kSymbol});
        }
    }
    return ret;
}

}  // namespace yacis::utility

#endif  // YACIS_UTILITY_TOKENIZER_HPP_
