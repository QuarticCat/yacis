#include <iostream>
#include <memory>
#include <string>

#include "tao/pegtl/contrib/parse_tree.hpp"
#include "yacis/analysis/check.hpp"
#include "yacis/analysis/replace.hpp"
#include "yacis/ast/node.hpp"
#include "yacis/ast/selector.hpp"
#include "yacis/utility/print_tree.hpp"

// Temporary main
int main() {
    auto in = tao::pegtl::file_input("../examples/pair.yac");
    auto root = tao::pegtl::parse_tree::parse<yacis::grammar::Grammar,
                                              yacis::ast::BaseNode,
                                              yacis::ast::Selector>(in);
    yacis::utility::print_tree(root);
    yacis::analysis::check(root);
    yacis::analysis::replace(root);
    yacis::utility::print_tree(root);
}
