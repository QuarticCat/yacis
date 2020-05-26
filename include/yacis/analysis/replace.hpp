#ifndef YACIS_ANALYSIS_REPLACE_HPP_
#define YACIS_ANALYSIS_REPLACE_HPP_

#include <any>
#include <memory>

#include "yacis/analysis/symbol_table.hpp"
#include "yacis/ast/node.hpp"

namespace yacis::analysis {

namespace internal {

class ReplaceVisitor: public ast::BaseVisitor {
  public:
    std::shared_ptr<SymbolTable<int32_t>> val_table =
        std::make_shared<SymbolTable<int32_t>>();
    std::shared_ptr<SymbolTable<size_t>> global_table =
        std::make_shared<SymbolTable<size_t>>();
    std::shared_ptr<SymbolTable<size_t>> arg_table =
        std::make_shared<SymbolTable<size_t>>();
    std::unique_ptr<ast::BaseNode>* curr_node = nullptr;
    size_t global_count = 0;
    size_t arg_count = 0;

    void call(std::unique_ptr<ast::BaseNode>& n) {
        auto temp = curr_node;
        curr_node = &n;
        n->accept(this);
        curr_node = temp;
    }

    std::any visit(ast::BaseNode& n) override {
        for (auto&& i : n.children) call(i);
        return std::any();
    }

    std::any visit(ast::IntLitNode& n) override {
        *curr_node = std::make_unique<ast::ValNode>(n.info.value);
        return std::any();
    }

    std::any visit(ast::BoolLitNode& n) override {
        *curr_node = std::make_unique<ast::ValNode>(n.info.value);
        return std::any();
    }

    std::any visit(ast::CharLitNode& n) override {
        *curr_node = std::make_unique<ast::ValNode>(n.info.value);
        return std::any();
    }

    std::any visit(ast::VarNameNode& n) override {
        auto& name = n.info.name;
        if (arg_table->contains(name))
            *curr_node = std::make_unique<ast::ArgNode>(arg_count - 1 -
                                                        (*arg_table)[name]);
        else if (val_table->contains(name))
            *curr_node = std::make_unique<ast::ValNode>((*val_table)[name]);
        else
            *curr_node =
                std::make_unique<ast::GlobalNode>((*global_table)[name]);
        return std::any();
    }

    std::any visit(ast::ApplExprNode& n) override {
        for (auto&& i : n.children) call(i);
        return std::any();
    }

    std::any visit(ast::CondExprNode& n) override {
        call(n.children[0]);
        call(n.children[1]);
        call(n.children[2]);
        return std::any();
    }

    std::any visit(ast::LambdaParamNode& n) override {
        auto& name = ast::as<ast::VarNameNode>(n.children[0]).info.name;
        (*arg_table)[name] = arg_count++;
        return std::any();
    }

    std::any visit(ast::LambdaExprNode& n) override {
        arg_table = arg_table->new_child();
        for (auto&& i : n.children) call(i);
        arg_table = arg_table->parent;
        arg_count -= n.children.size() - 1;
        return std::any();
    }

    std::any visit(ast::ValueAssignNode& n) override {
        call(n.children[1]);
        auto& name = ast::as<ast::VarNameNode>(n.children[0]).info.name;
        if (n.children[1]->tag == ast::NodeTag::kVal)
            (*val_table)[name] = ast::as<ast::ValNode>(n.children[1]).value;
        else
            (*global_table)[name] = global_count++;
        return std::any();
    }

    std::any visit(ast::OutputNode& n) override {
        call(n.children[0]);
        return std::any();
    }
};

/**
 * @brief Analysis stage 2. Replace constants, arguments and global variables.
 * @param root Root node of AST.
 */
inline void replace(std::unique_ptr<ast::BaseNode>& root) {
    ReplaceVisitor().call(root);
}

}  // namespace internal

using internal::replace;

}  // namespace yacis::analysis

#endif  // YACIS_ANALYSIS_REPLACE_HPP_
