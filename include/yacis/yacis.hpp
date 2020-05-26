#ifndef YACIS_YACIS_HPP_
#define YACIS_YACIS_HPP_

#include <string>

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

namespace yacis {

inline std::string compile_to_asm(std::string path) {
    auto in = tao::pegtl::file_input(path);
    auto root = tao::pegtl::parse_tree::parse<yacis::grammar::Grammar,
                                              yacis::ast::BaseNode,
                                              yacis::ast::Selector>(in);
    yacis::analysis::check(root);
    yacis::analysis::replace(root);
    auto output = yacis::analysis::eval(root);

    // clang-format off
    std::string ret = "main:";
    for (const auto& i : output) {
        if (i.second == analysis::t_int) {
            uint32_t val = i.first; // convert to unsigned for bit operations
            ret += "\n\taddi $v0, $zero, 1"
                   "\n\tlui $a0, " + std::to_string((val & 0xffff0000) >> 16u) +
                   "\n\taddi $a0, $a0, " + std::to_string(val & 0xffff0000) +
                   "\n\tsyscall";
        } else if (i.second == analysis::t_char) {
            char val = i.first;
            ret += "\n\taddi $v0, $zero, 11"
                   "\n\taddi $a0, $zero, " + std::to_string(val) +
                   "\n\tsyscall";
        } else {
            if (i.first)
                ret += "\n\taddi $v0, $zero, 11"
                       "\n\taddi $a0, $zero, " + std::to_string('T') +
                       "\n\tsyscall"
                       "\n\taddi $a0, $zero, " + std::to_string('r') +
                       "\n\tsyscall"
                       "\n\taddi $a0, $zero, " + std::to_string('u') +
                       "\n\tsyscall"
                       "\n\taddi $a0, $zero, " + std::to_string('e') +
                       "\n\tsyscall";
            else
                ret += "\n\taddi $v0, $zero, 11"
                       "\n\taddi $a0, $zero, " + std::to_string('F') +
                       "\n\tsyscall"
                       "\n\taddi $a0, $zero, " + std::to_string('a') +
                       "\n\tsyscall"
                       "\n\taddi $a0, $zero, " + std::to_string('l') +
                       "\n\tsyscall"
                       "\n\taddi $a0, $zero, " + std::to_string('s') +
                       "\n\tsyscall"
                       "\n\taddi $a0, $zero, " + std::to_string('e') +
                       "\n\tsyscall";
        }
    }
    // clang-format on

    return ret;
}

}  // namespace yacis

#endif  // YACIS_YACIS_HPP_
