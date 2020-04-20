#ifndef YACIS_NODE_HPP_
#define YACIS_NODE_HPP_

#include <memory>
#include <vector>

#include "tao/pegtl.hpp"
#include "yacis/type.hpp"

namespace yacis::ast {

// Forward declaration
class BaseNode;

class BaseVisitor {
  public:
    virtual void visit(BaseNode&) = 0;
};

enum class NodeTag {
    kRoot,

    kIntLit,
    kBoolLit,
    kCharLit,

    kVarName,

    kTypeName,
    kType,

    kApplExpr,
    kCondExpr,
    kLetExpr,
    kLambdaParam,
    kLambdaExpr,
    kExpression,

    kTypeAlias,
    kTypeAssign,
    kValueAssign,
    kOutput
};

class BaseNode {
    // Modified from tao::pegtl::parse_tree::basic_node
  public:
    using children_t = std::vector<std::unique_ptr<BaseNode>>;
    using iterator_t = tao::pegtl::internal::iterator;

    NodeTag tag = NodeTag::kRoot;
    children_t children;
    iterator_t m_begin;
    iterator_t m_end;

    BaseNode() = default;
    BaseNode(const BaseNode&) = delete;
    BaseNode(BaseNode&&) = delete;
    BaseNode& operator=(const BaseNode&) = delete;
    BaseNode& operator=(BaseNode&&) = delete;
    virtual ~BaseNode() = default;

    BaseNode(BaseNode&& base_node, NodeTag node_tag);

    template<typename Rule, typename Input, typename... States>
    void start(const Input& in, States&&...) {
        m_begin = iterator_t(in.iterator());
    }

    template<typename Rule, typename Input, typename... States>
    void success(const Input& in, States&&...) noexcept {
        m_end = iterator_t(in.iterator());
    }

    template<typename Rule, typename Input, typename... States>
    void failure(const Input&, States&&...) noexcept {};

    template<typename... States>
    void emplace_back(std::unique_ptr<BaseNode> child, States&&...) {
        children.emplace_back(std::move(child));
    }

    virtual void accept(BaseVisitor* visitor) {
        visitor->visit(*this);
    }
};

template<NodeTag Tag, typename Info = void>
class Node: public BaseNode {
  public:
    Info info;

    explicit Node(BaseNode&& base_node): BaseNode(std::move(base_node), Tag){};

    void accept(BaseVisitor* visitor) override {
        visitor->visit(*this);
    }
};

template<NodeTag Tag>
class Node<Tag, void>: public BaseNode {
  public:
    explicit Node(BaseNode&& base_node): BaseNode(std::move(base_node), Tag){};

    void accept(BaseVisitor* visitor) override {
        visitor->visit(*this);
    }
};

struct IntLitInfo {
    int32_t value;
};

struct BoolLitInfo {
    bool value;
};

struct CharLitInfo {
    char value;
};

struct VarNameInfo {
    std::string name;
};

struct TypeNameInfo {
    std::string name;
    Type type;
};

struct TypeInfo {
    Type type;
};

struct ExpressionInfo {
    Type type;
};

using IntLitNode = Node<NodeTag::kIntLit, IntLitInfo>;
using BoolLitNode = Node<NodeTag::kBoolLit, BoolLitInfo>;
using CharLitNode = Node<NodeTag::kCharLit, CharLitInfo>;

using VarNameNode = Node<NodeTag::kVarName, VarNameInfo>;

using TypeNameNode = Node<NodeTag::kTypeName, TypeNameInfo>;
using TypeNode = Node<NodeTag::kType, TypeInfo>;

using ApplExprNode = Node<NodeTag::kApplExpr>;
using CondExprNode = Node<NodeTag::kCondExpr>;
using LetExprNode = Node<NodeTag::kLetExpr>;
using LambdaParamNode = Node<NodeTag::kLambdaParam>;
using LambdaNode = Node<NodeTag::kLambdaExpr>;
using ExpressionNode = Node<NodeTag::kExpression, ExpressionInfo>;

using TypeAliasNode = Node<NodeTag::kTypeAlias>;
using TypeAssignNode = Node<NodeTag::kTypeAssign>;
using ValueAssignNode = Node<NodeTag::kValueAssign>;
using OutputNode = Node<NodeTag::kOutput>;

}  // namespace yacis::ast

#endif  // YACIS_NODE_HPP_
