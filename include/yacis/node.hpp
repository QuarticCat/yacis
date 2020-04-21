#ifndef YACIS_NODE_HPP_
#define YACIS_NODE_HPP_

#include <any>
#include <memory>
#include <vector>

#include "tao/pegtl.hpp"
#include "yacis/symbol_table.hpp"
#include "yacis/type.hpp"

namespace yacis::ast {

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

    kTypeAlias,
    kTypeAssign,
    kValueAssign,
    kOutput
};

class BaseNode;

template<NodeTag Tag, typename Info = void>
class Node;

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
};

using IntLitNode = Node<NodeTag::kIntLit, IntLitInfo>;
using BoolLitNode = Node<NodeTag::kBoolLit, BoolLitInfo>;
using CharLitNode = Node<NodeTag::kCharLit, CharLitInfo>;

using VarNameNode = Node<NodeTag::kVarName, VarNameInfo>;

using TypeNameNode = Node<NodeTag::kTypeName, TypeNameInfo>;
using TypeNode = Node<NodeTag::kType>;

using ApplExprNode = Node<NodeTag::kApplExpr>;
using CondExprNode = Node<NodeTag::kCondExpr>;
using LetExprNode = Node<NodeTag::kLetExpr>;
using LambdaParamNode = Node<NodeTag::kLambdaParam>;
using LambdaExprNode = Node<NodeTag::kLambdaExpr>;

using TypeAliasNode = Node<NodeTag::kTypeAlias>;
using TypeAssignNode = Node<NodeTag::kTypeAssign>;
using ValueAssignNode = Node<NodeTag::kValueAssign>;
using OutputNode = Node<NodeTag::kOutput>;

class BaseVisitor {
  public:
    virtual std::any visit(BaseNode&) = 0;

    virtual std::any visit(IntLitNode&) = 0;
    virtual std::any visit(BoolLitNode&) = 0;
    virtual std::any visit(CharLitNode&) = 0;

    virtual std::any visit(VarNameNode&) = 0;

    virtual std::any visit(TypeNameNode&) = 0;
    virtual std::any visit(TypeNode&) = 0;

    virtual std::any visit(ApplExprNode&) = 0;
    virtual std::any visit(CondExprNode&) = 0;
    virtual std::any visit(LetExprNode&) = 0;
    virtual std::any visit(LambdaParamNode&) = 0;
    virtual std::any visit(LambdaExprNode&) = 0;

    virtual std::any visit(TypeAliasNode&) = 0;
    virtual std::any visit(TypeAssignNode&) = 0;
    virtual std::any visit(ValueAssignNode&) = 0;
    virtual std::any visit(OutputNode&) = 0;
};

class BaseNode {
    // Modified from tao::pegtl::parse_tree::basic_node
  public:
    using children_t = std::vector<std::unique_ptr<BaseNode>>;
    using iterator_t = tao::pegtl::internal::iterator;

    NodeTag tag = NodeTag::kRoot;
    BaseNode* parent = nullptr;  // should be observer_ptr
    children_t children;
    iterator_t m_begin;
    iterator_t m_end;

    BaseNode() = default;
    BaseNode(const BaseNode&) = delete;
    BaseNode(BaseNode&&) = delete;
    BaseNode& operator=(const BaseNode&) = delete;
    BaseNode& operator=(BaseNode&&) = delete;
    virtual ~BaseNode() = default;

    /**
     * @brief Construct base node with given base node and node tag. This
     *        constructor is to simplify the construction of derived nodes.
     */
    BaseNode(BaseNode&& base_node, NodeTag node_tag):
        tag(node_tag),
        parent(base_node.parent),
        children(std::move(base_node.children)),
        m_begin(base_node.m_begin),
        m_end(base_node.m_end){};

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
        child->parent = this;
        children.emplace_back(std::move(child));
    }

    virtual std::any accept(BaseVisitor* visitor) {
        return visitor->visit(*this);
    }
};

template<NodeTag Tag, typename Info>
class Node: public BaseNode {
  public:
    Info info;

    explicit Node(BaseNode&& base_node): BaseNode(std::move(base_node), Tag){};

    std::any accept(BaseVisitor* visitor) override {
        return visitor->visit(*this);
    }
};

template<NodeTag Tag>
class Node<Tag, void>: public BaseNode {
  public:
    explicit Node(BaseNode&& base_node): BaseNode(std::move(base_node), Tag){};

    std::any accept(BaseVisitor* visitor) override {
        return visitor->visit(*this);
    }
};

}  // namespace yacis::ast

#endif  // YACIS_NODE_HPP_
