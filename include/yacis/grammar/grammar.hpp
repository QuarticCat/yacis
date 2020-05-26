#ifndef YACIS_GRAMMAR_GRAMMAR_HPP_
#define YACIS_GRAMMAR_GRAMMAR_HPP_

#include "tao/pegtl.hpp"

namespace yacis::grammar {

// Wait for PEGTL to fix bugs and improve support for custom rules.

// struct State {
//    int32_t paren_level;
//};

namespace internal {

using namespace tao::pegtl;

// Forward declarations
struct Expression;
struct TypeAssign;
struct ValueAssign;

/**
 * @brief Consumes a comment but does not consume newline characters.
 */
struct Comment: seq<string<'-', '-'>, star<not_at<eolf>, any>> {};

/**
 * @brief Cross-line separator. Consumes one or more whitespace characters or
 *        comments.
 */
struct CSep: plus<sor<space, Comment>> {};

/**
 * @brief Alternative cross-line separator. Consumes zero or more whitespace
 *        characters or comments.
 */
struct CSep0: star<sor<space, Comment>> {};

///**
// * @brief Inline separator helper. Consumes one or more ' ' or '\t'.
// */
// struct ISepHelper: plus<blank> {};
//
///**
// * @brief Alternative inline separator helper. Consumes zero or more ' ' or
// *        '\t'.
// */
// struct ISep0Helper: star<blank> {};
//
///**
// * @brief Inline separator. But behaves as CSep when within parenthesis.
// */
// struct ISep {
//    template<apply_mode A,
//             rewind_mode M,
//             template<typename...>
//             class Action,
//             template<typename...>
//             class Control,
//             typename Input>
//    [[nodiscard]] static bool match(Input& in, State& state) {
//        if (state.paren_level > 0)
//            return CSep::match<A, M, Action, Control>(in, state);
//        else
//            return ISepHelper::match<A, M, Action, Control>(in, state);
//    }
//};
//
///**
// * @brief Alternative inline separator. But behaves as CSep0 when within
// *        parenthesis.
// */
// struct ISep0 {
//    template<apply_mode A,
//             rewind_mode M,
//             template<typename...>
//             class Action,
//             template<typename...>
//             class Control,
//             typename Input>
//    [[nodiscard]] static bool match(Input& in, State& state) {
//        if (state.paren_level > 0)
//            return CSep0::match<A, M, Action, Control>(in, state);
//        else
//            return ISep0Helper::match<A, M, Action, Control>(in, state);
//    }
//};

/**
 * @brief Inline separator. Consumes one or more ' ' or '\t'.
 */
struct ISep: plus<blank> {};

/**
 * @brief Alternative inline separator. Consumes zero or more ' ' or '\t'.
 */
struct ISep0: star<blank> {};

/**
 * @brief Consumes zero or more ' ' or '\t' or comment and an eol/eof.
 */
struct ToEolf: seq<ISep0, opt<Comment>, eolf> {};

///**
// * @brief Consumes a left parenthesis and add 1 to paren_level.
// */
// struct LParen {
//    template<apply_mode A,
//             rewind_mode M,
//             template<typename...>
//             class Action,
//             template<typename...>
//             class Control,
//             typename Input>
//    [[nodiscard]] static bool match(Input& in, State& state) {
//        if (one<'('>::match<A, M, Action, Control>(in, state))
//            ++state.paren_level;
//    }
//};
//
///**
// * @brief Consumes a right parenthesis and subtract 1 from paren_level.
// */
// struct RParen {
//    template<apply_mode A,
//             rewind_mode M,
//             template<typename...>
//             class Action,
//             template<typename...>
//             class Control,
//             typename Input>
//    [[nodiscard]] static bool match(Input& in, State& state) {
//        if (one<')'>::match<A, M, Action, Control>(in, state))
//            --state.paren_level;
//    }
//};
//
///**
// * @brief Consumes a pair of parenthesis with designated content within it.
// * @tparam Content The content that within the parenthesis.
// */
// template<typename Content>
// using Paren = seq<LParen, CSep0, Content, CSep0, RParen>;

/**
 * @brief Consumes a pair of parenthesis with designated content within it.
 * @tparam Content The content that within the parenthesis.
 */
template<typename Content>
using Paren = seq<one<'('>, CSep0, Content, CSep0, one<')'>>;

/**
 * @brief Int literal. Consumes one or more digits.
 */
struct IntLit: seq<opt<one<'-'>>, plus<digit>> {};

/**
 * @brief Bool literal. Consumes "True" or "False".
 */
struct BoolLit:
    sor<string<'T', 'r', 'u', 'e'>, string<'F', 'a', 'l', 's', 'e'>> {};

/**
 * @brief Escaped char literal.
 */
struct Escaped:
    if_must<one<'\\'>,
            one<'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '\'', '\"', '0'>> {};

/**
 * @brief Char literal.
 */
struct CharLit: if_must<one<'\''>, sor<Escaped, not_one<'\''>>, one<'\''>> {};

/**
 * @brief Any kind of literal.
 */
struct Literal: sor<IntLit, BoolLit, CharLit> {};

/**
 * @brief Lower case identifier. [a-z_][a-zA-Z0-9_]*
 */
struct LCID: seq<sor<lower, one<'_'>>, star<identifier_other>> {};

/**
 * @brief Keyword, namely the name that variables can not use.
 */
struct Keyword:
    sor<string<'i', 'f'>,
        string<'t', 'h', 'e', 'n'>,
        string<'e', 'l', 's', 'e'>,
        string<'l', 'e', 't'>,
        string<'i', 'n'>,
        string<'d', 'a', 't', 'a'>> {};

/**
 * @brief Variable name.
 */
struct VarName: minus<LCID, Keyword> {};

/**
 * @brief Upper case identifier. [A-Z][a-zA-Z0-9_]*
 */
struct UCID: seq<upper, star<identifier_other>> {};

/**
 * @brief Type name. Just an alias of UCID. Note that it doesn't equal to basic
 *        type because of type aliases.
 */
struct TypeName: UCID {};

/**
 * @brief Type. Includes function type and single type name. It is meaningless
 *        to distinguish function type and basic type during parsing because of
 *        type aliases.
 */
struct Type:
    list<sor<Paren<Type>, TypeName>, seq<ISep0, one<'-'>, one<'>'>, CSep0>> {};

/**
 * @brief Application expression. Single expression, variable or literal is also
 *        application expression in paring stage.
 */
struct ApplExpr: list<sor<Paren<Expression>, VarName, Literal>, ISep> {};

/**
 * @brief Conditional (if-then-else) expression.
 */
struct CondExpr:
    seq<string<'i', 'f'>,
        CSep,
        Expression,
        CSep,
        string<'t', 'h', 'e', 'n'>,
        CSep,
        Expression,
        CSep,
        string<'e', 'l', 's', 'e'>,
        CSep,
        Expression> {};

/**
 * @brief Let-in expression.
 */
struct LetExpr: failure {};
// struct LetExpr:
//     seq<string<'l', 'e', 't'>,
//         CSep,
//         list<sor<TypeAssign, ValueAssign>, CSep0>,
//         CSep,
//         string<'i', 'n'>,
//         CSep,
//         Expression> {};

/**
 * @brief Lambda parameter.
 */
struct LambdaParam:
    if_must<VarName, CSep0, one<':'>, CSep0, sor<Paren<Type>, TypeName>> {};

/**
 * @brief Lambda expression.
 */
struct LambdaExpr:
    if_must<one<'\\'>,
            CSep0,
            list<LambdaParam, CSep>,
            CSep0,
            one<'-'>,
            one<'>'>,
            CSep0,
            Expression> {};

/**
 * @brief All kinds of expressions. Some cases are contained in ApplExpr.
 */
struct Expression: sor<ApplExpr, CondExpr, LambdaExpr, LetExpr> {};

/**
 * @brief Type alias statement.
 */
struct TypeAlias:
    seq<string<'d', 'a', 't', 'a'>,
        ISep,
        TypeName,
        ISep0,
        one<'='>,
        CSep0,
        Type,
        ToEolf> {};

/**
 * @brief Type assignment statement.
 */
struct TypeAssign: seq<VarName, ISep0, one<':'>, CSep0, Type, ToEolf> {};

/**
 * @brief Value assignment statement.
 */
struct ValueAssign: seq<VarName, ISep0, one<'='>, CSep0, Expression, ToEolf> {};

/**
 * @brief Output statement.
 */
struct Output: seq<Expression, ToEolf> {};

/**
 * @brief Whole grammar of this language.
 */
struct Grammar:
    must<list<CSep0, sor<TypeAlias, TypeAssign, ValueAssign, Output>>, eof> {};

}  // namespace internal

// clang-format off
using internal::Comment;
using internal::CSep;
using internal::CSep0;
using internal::ISep;
using internal::ISep0;
using internal::ToEolf;

using internal::IntLit;
using internal::BoolLit;
using internal::CharLit;
using internal::Literal;

using internal::Keyword;
using internal::VarName;

using internal::TypeName;
using internal::Type;

using internal::ApplExpr;
using internal::CondExpr;
using internal::LetExpr;
using internal::LambdaParam;
using internal::LambdaExpr;
using internal::Expression;

using internal::TypeAlias;
using internal::TypeAssign;
using internal::ValueAssign;
using internal::Output;
using internal::Grammar;
// clang-format on

}  // namespace yacis::grammar

#endif  // YACIS_GRAMMAR_GRAMMAR_HPP_
