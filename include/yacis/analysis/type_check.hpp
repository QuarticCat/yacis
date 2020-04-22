#ifndef YACIS_TYPE_CHECK_HPP_
#define YACIS_TYPE_CHECK_HPP_

#include <any>
#include <memory>
#include <stdexcept>
#include <vector>

#include "yacis/analysis/error.hpp"
#include "yacis/analysis/symbol_table.hpp"
#include "yacis/analysis/type.hpp"
#include "yacis/ast/node.hpp"

namespace yacis::analysis {

namespace internal {

inline const std::map<std::string, Type> init_type{
    {"Int", t_int},    // int32_t
    {"Bool", t_bool},  // bool
    {"Char", t_char},  // char

    {"negate", {t_int, t_int}},         // - (unary)
    {"add", {t_int, t_int, t_int}},     // +
    {"sub", {t_int, t_int, t_int}},     // - (binary)
    {"mul", {t_int, t_int, t_int}},     // *
    {"div", {t_int, t_int, t_int}},     // /
    {"mod", {t_int, t_int, t_int}},     // %
    {"eq", {t_int, t_int, t_bool}},     // ==
    {"neq", {t_int, t_int, t_bool}},    // !=
    {"lt", {t_int, t_int, t_bool}},     // <
    {"gt", {t_int, t_int, t_bool}},     // >
    {"leq", {t_int, t_int, t_bool}},    // <=
    {"geq", {t_int, t_int, t_bool}},    // >=
    {"and", {t_bool, t_bool, t_bool}},  // &&
    {"or", {t_bool, t_bool, t_bool}},   // ||
    {"not", {t_bool, t_bool}}           // !
};

inline const std::map<std::string, bool> init_defined{
    {"negate", true},  // - (unary)
    {"add", true},     // +
    {"sub", true},     // - (binary)
    {"mul", true},     // *
    {"div", true},     // /
    {"mod", true},     // %
    {"eq", true},      // ==
    {"neq", true},     // !=
    {"lt", true},      // <
    {"gt", true},      // >
    {"leq", true},     // <=
    {"geq", true},     // >=
    {"and", true},     // &&
    {"or", true},      // ||
    {"not", true}      // !
};

class TypeCheckVisitor: public ast::BaseVisitor {
  public:
    std::shared_ptr<SymbolTable<Type>> type_table =
        std::make_shared<SymbolTable<Type>>(init_type);
    std::shared_ptr<SymbolTable<bool>> defined_table =
        std::make_shared<SymbolTable<bool>>(init_defined);

    std::any visit(ast::BaseNode& n) override {
        for (auto&& i : n.children) i->accept(this);
        return std::any();
    }

    std::any visit(ast::IntLitNode&) override {
        return t_int;
    }

    std::any visit(ast::BoolLitNode&) override {
        return t_bool;
    }

    std::any visit(ast::CharLitNode&) override {
        return t_char;
    }

    std::any visit(ast::VarNameNode& n) override {
        auto name = n.info.name;
        if (!defined_table->contains(name))
            fatal_define_error(n.m_begin, "Variable hasn't been defined.");
        if (!type_table->contains(name))
            fatal_define_error(n.m_begin,
                               "Variable hasn't been assigned type.");
        return (*type_table)[name];
    }

    std::any visit(ast::TypeNameNode& n) override {
        auto name = n.info.name;
        if (type_table->contains(name))
            return (*type_table)[name];
        else
            fatal_type_error(n.m_begin, "Type name doesn't exist.");
    }

    std::any visit(ast::TypeNode& n) override {
        std::vector<Type> type_vec;
        type_vec.reserve(n.children.size());
        for (auto&& i : n.children) {
            type_vec.push_back(std::any_cast<Type>(i->accept(this)));
        }
        Type type(type_vec);
        type.flatten();
        return type;
    }

    std::any visit(ast::ApplExprNode& n) override {
        auto it = n.children.begin();
        Type func_type = std::any_cast<Type>((*it)->accept(this));
        for (++it; it != n.children.end(); ++it) {
            try {
                func_type.apply(std::any_cast<Type>((*it)->accept(this)));
            } catch (const std::invalid_argument&) {
                fatal_type_error((*it)->m_begin, "Not applicable");
            }
        }
        return func_type;
    }

    std::any visit(ast::CondExprNode& n) override {
        auto if_type = std::any_cast<Type>(n.children[0]->accept(this));
        if (if_type.tag == TypeTag::kFunction)
            fatal_type_error(n.children[0]->m_begin,
                             "If-expression can not be function.");
        auto then_type = std::any_cast<Type>(n.children[1]->accept(this));
        auto else_type = std::any_cast<Type>(n.children[2]->accept(this));
        if (then_type != else_type)
            fatal_type_error(n.children[1]->m_begin,
                             "The type of then-expression should be the same as"
                             "the type of else-expression.");
        return then_type;
    }

    std::any visit(ast::LetExprNode& n) override {
        std::any ret;

        type_table = type_table->new_child();
        defined_table = defined_table->new_child();
        for (auto&& i : n.children) ret = i->accept(this);
        type_table = type_table->parent;
        defined_table = defined_table->parent;

        return std::any_cast<Type>(ret);
    }

    std::any visit(ast::LambdaParamNode& n) override {
        auto name = ast::get_node<ast::TypeNameNode>(n.children[0]).info.name;
        auto type = std::any_cast<Type>(n.children[1]->accept(this));
        (*type_table)[name] = type;
        (*defined_table)[name] = true;
        return type;
    }

    std::any visit(ast::LambdaExprNode& n) override {
        std::vector<Type> type_vec;
        type_vec.reserve(n.children.size());

        type_table = type_table->new_child();
        defined_table = defined_table->new_child();
        for (auto&& i : n.children) {
            type_vec.push_back(std::any_cast<Type>(i->accept(this)));
        }
        type_table = type_table->parent;
        defined_table = defined_table->parent;

        return Type(type_vec);
    }

    std::any visit(ast::TypeAliasNode& n) override {
        auto name = ast::get_node<ast::TypeNameNode>(n.children[0]).info.name;
        if (type_table->i_contains(name))
            fatal_type_error(n.children[0]->m_begin,
                             "Type name has already been defined.");
        auto type = std::any_cast<Type>(n.children[1]->accept(this));
        (*type_table)[name] = type;
        return std::any();
    }

    std::any visit(ast::TypeAssignNode& n) override {
        auto name = ast::get_node<ast::VarNameNode>(n.children[0]).info.name;
        if (type_table->i_contains(name))
            fatal_type_error(n.children[0]->m_begin,
                             "Variable has already been assigned type.");
        auto type = std::any_cast<Type>(n.children[1]->accept(this));
        (*type_table)[name] = type;
        return std::any();
    }

    std::any visit(ast::ValueAssignNode& n) override {
        auto name = ast::get_node<ast::VarNameNode>(n.children[0]).info.name;
        if (defined_table->i_contains(name))
            fatal_define_error(n.children[0]->m_begin,
                               "Variable has already been defined.");
        (*defined_table)[name] = true;
        auto type = std::any_cast<Type>(n.children[1]->accept(this));
        if (type_table->i_contains(name)) {
            if ((*type_table)[name] != type)
                fatal_type_error(n.children[1]->m_begin,
                                 "Can not match the assigned type.");
        } else {
            (*type_table)[name] = type;
        }
        return std::any();
    }

    std::any visit(ast::OutputNode& n) override {
        n.children[0]->accept(this);
        return std::any();
    }
};

/**
 * @brief Analysis stage 1. Check if all types are correct. Directly output
 *        error message and close the program.
 * @param root Root node of AST.
 */
void check_type(std::unique_ptr<ast::BaseNode>& root) {
    TypeCheckVisitor visitor;
    root->accept(&visitor);
}

}  // namespace internal

// clang-format off
using internal::TypeCheckVisitor;
using internal::check_type;
// clang-format on

}  // namespace yacis::analysis

#endif  // YACIS_TYPE_CHECK_HPP_
