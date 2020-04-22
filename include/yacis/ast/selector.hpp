#ifndef YACIS_SELECTOR_HPP_
#define YACIS_SELECTOR_HPP_

#include <memory>
#include <string>
#include <type_traits>

#include "yacis/ast/node.hpp"
#include "yacis/grammar/grammar.hpp"

namespace yacis::ast {

namespace internal {

/**
 * @brief Convert n to designated type.
 * @tparam Node Target node type
 * @param n Node ptr to be converted
 */
template<typename Node>
void convert_node(std::unique_ptr<BaseNode>& n) {
    n = std::make_unique<Node>(std::move(*n.release()));
}

/**
 * @brief Replace n with its first child. This function does not check if it has
 *        no child.
 * @param n Node to be folded
 */
inline void fold_node(std::unique_ptr<BaseNode>& n) {
    n = std::move(n->children[0]);
}

template<typename Rule>
struct Selector: std::false_type {};

template<>
struct Selector<grammar::IntLit>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<IntLitNode>(n);
        auto& node = get_node<IntLitNode>(n);

        uint64_t num = 0;
        bool is_negative = false;
        const char* b = node.m_begin.data;
        const char* e = node.m_end.data;
        if (*b == '-') {
            is_negative = true;
            ++b;
        }
        for (; b != e; ++b) num = (num * 10 + *b - '0') & ((1u << 31u) - 1);
        node.info.value = static_cast<int32_t>(is_negative ? -num : num);
    }
};

template<>
struct Selector<grammar::BoolLit>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<BoolLitNode>(n);
        auto& node = get_node<BoolLitNode>(n);

        node.info.value = *node.m_begin.data == 'T';
    }
};

template<>
struct Selector<grammar::CharLit>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<CharLitNode>(n);
        auto& node = get_node<CharLitNode>(n);

        const char* b = node.m_begin.data;
        if (*b != '\\')
            node.info.value = *b;
        else
            switch (*(b + 1)) {
            case 'a':
                node.info.value = '\a';
                break;
            case 'b':
                node.info.value = '\b';
                break;
            case 'f':
                node.info.value = '\f';
                break;
            case 'n':
                node.info.value = '\n';
                break;
            case 'r':
                node.info.value = '\r';
                break;
            case 't':
                node.info.value = '\t';
                break;
            case 'v':
                node.info.value = '\v';
                break;
            case '\\':
                node.info.value = '\\';
                break;
            case '\'':
                node.info.value = '\'';
                break;
            case '\"':
                node.info.value = '\"';
                break;
            case '0':
                node.info.value = '\0';
                break;
            }
    }
};

template<>
struct Selector<grammar::VarName>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<VarNameNode>(n);
        auto& node = get_node<VarNameNode>(n);

        node.info.name = std::string(node.m_begin.data, node.m_end.data);
    }
};

template<>
struct Selector<grammar::TypeName>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<TypeNameNode>(n);
        auto& node = get_node<TypeNameNode>(n);

        node.info.name = std::string(node.m_begin.data, node.m_end.data);
    }
};

template<>
struct Selector<grammar::Type>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        if (n->children.size() == 1 && n->children[0]->tag == NodeTag::kType)
            fold_node(n);
        else
            convert_node<TypeNode>(n);
    }
};

template<>
struct Selector<grammar::ApplExpr>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        if (n->children.size() == 1)
            fold_node(n);
        else
            convert_node<ApplExprNode>(n);
    }
};

template<>
struct Selector<grammar::CondExpr>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<CondExprNode>(n);
    }
};

template<>
struct Selector<grammar::LetExpr>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<LetExprNode>(n);
    }
};

template<>
struct Selector<grammar::LambdaParam>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<LambdaParamNode>(n);
    }
};

template<>
struct Selector<grammar::LambdaExpr>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<LambdaExprNode>(n);
    }
};

template<>
struct Selector<grammar::Expression>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        fold_node(n);
    }
};

template<>
struct Selector<grammar::TypeAlias>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<TypeAliasNode>(n);
    }
};

template<>
struct Selector<grammar::TypeAssign>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<TypeAssignNode>(n);
    }
};

template<>
struct Selector<grammar::ValueAssign>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<ValueAssignNode>(n);
    }
};

template<>
struct Selector<grammar::Output>: std::true_type {
    template<typename... States>
    static void transform(std::unique_ptr<BaseNode>& n, States&&...) {
        convert_node<OutputNode>(n);
    }
};

}  // namespace internal

using internal::Selector;

}  // namespace yacis::ast

#endif  // YACIS_SELECTOR_HPP_
