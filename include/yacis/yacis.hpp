#ifndef YACIS_YACIS_HPP_
#define YACIS_YACIS_HPP_

#include <string>
#include <utility>

#include "tao/pegtl/contrib/parse_tree.hpp"
#include "yacis/analysis/check.hpp"
#include "yacis/analysis/error.hpp"
#include "yacis/analysis/eval.hpp"
#include "yacis/analysis/replace.hpp"
#include "yacis/analysis/symbol_table.hpp"
#include "yacis/analysis/type.hpp"
#include "yacis/analysis/yac_obj.hpp"
#include "yacis/ast/node.hpp"
#include "yacis/ast/selector.hpp"
#include "yacis/grammar/grammar.hpp"
#include "yacis/utility/print_tree.hpp"
#include "yacis/utility/tokenizer.hpp"

namespace yacis {

using file_input = tao::pegtl::file_input<>;
using string_input = tao::pegtl::string_input<>;

template<typename Input>
inline std::vector<std::pair<int32_t, analysis::Type>>
compile_to_output(Input&& input) {
    try {
        auto root = tao::pegtl::parse_tree::
            parse<grammar::Grammar, ast::BaseNode, ast::Selector>(
                std::forward<Input>(input));
        analysis::check(root);
        analysis::replace(root);
        return analysis::eval(root);
    } catch (const tao::pegtl::parse_error& e) {
        throw analysis::ParseError(e.positions[0], "Syntax error.");
    }
}

template<typename Input>
inline std::string compile_to_asm(Input&& input) {
    auto output = compile_to_output(std::forward<Input>(input));
    // clang-format off
    std::string ret = "main:";
    for (const auto& i : output) {
        if (i.second == analysis::t_int) {
            uint32_t val = i.first;  // convert to unsigned for bit operations
            if (val > 0x0000ffffu)
                ret += "\n\taddiu $v0, $zero, 1"
                       "\n\tlui $a0, " +
                       std::to_string((val & 0xffff0000u) >> 16u) +
                       "\n\taddiu $a0, $a0, " +
                       std::to_string(val & 0x0000ffffu) +
                       "\n\tsyscall";
            else
                ret += "\n\taddiu $v0, $zero, 1"
                       "\n\taddiu $a0, $zero, " +
                       std::to_string(val & 0x0000ffffu) +
                       "\n\tsyscall";
        } else if (i.second == analysis::t_char) {
            char val = i.first;
            ret += "\n\taddiu $v0, $zero, 11"
                   "\n\taddiu $a0, $zero, " + std::to_string(val) +
                   "\n\tsyscall";
        } else {
            if (i.first)
                ret += "\n\taddiu $v0, $zero, 11"
                       "\n\taddiu $a0, $zero, " + std::to_string('T') +
                       "\n\tsyscall"
                       "\n\taddiu $a0, $zero, " + std::to_string('r') +
                       "\n\tsyscall"
                       "\n\taddiu $a0, $zero, " + std::to_string('u') +
                       "\n\tsyscall"
                       "\n\taddiu $a0, $zero, " + std::to_string('e') +
                       "\n\tsyscall";
            else
                ret += "\n\taddiu $v0, $zero, 11"
                       "\n\taddiu $a0, $zero, " + std::to_string('F') +
                       "\n\tsyscall"
                       "\n\taddiu $a0, $zero, " + std::to_string('a') +
                       "\n\tsyscall"
                       "\n\taddiu $a0, $zero, " + std::to_string('l') +
                       "\n\tsyscall"
                       "\n\taddiu $a0, $zero, " + std::to_string('s') +
                       "\n\tsyscall"
                       "\n\taddiu $a0, $zero, " + std::to_string('e') +
                       "\n\tsyscall";
        }
    }
    // clang-format on
    return ret;
}

}  // namespace yacis

#endif  // YACIS_YACIS_HPP_
