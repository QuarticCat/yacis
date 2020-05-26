#ifndef YACIS_AST_NODE_HPP_
#define YACIS_AST_NODE_HPP_

#include <any>
#include <memory>
#include <vector>

#include "tao/pegtl.hpp"
#include "yacis/analysis/type.hpp"

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
    kOutput,

    // Special nodes
    kVal,
    kArg,
    kGlobal
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

struct OutputInfo {
    analysis::Type type;
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
using OutputNode = Node<NodeTag::kOutput, OutputInfo>;

class ValNode;
class ArgNode;
class GlobalNode;

class BaseVisitor {
  public:
    virtual ~BaseVisitor() = default;

    virtual std::any visit(BaseNode&) {
        return std::any();
    }

    virtual std::any visit(IntLitNode&) {
        return std::any();
    }

    virtual std::any visit(BoolLitNode&) {
        return std::any();
    }

    virtual std::any visit(CharLitNode&) {
        return std::any();
    }

    virtual std::any visit(VarNameNode&) {
        return std::any();
    }

    virtual std::any visit(TypeNameNode&) {
        return std::any();
    }

    virtual std::any visit(TypeNode&) {
        return std::any();
    }

    virtual std::any visit(ApplExprNode&) {
        return std::any();
    }

    virtual std::any visit(CondExprNode&) {
        return std::any();
    }

    virtual std::any visit(LetExprNode&) {
        return std::any();
    }

    virtual std::any visit(LambdaParamNode&) {
        return std::any();
    }

    virtual std::any visit(LambdaExprNode&) {
        return std::any();
    }

    virtual std::any visit(TypeAliasNode&) {
        return std::any();
    }

    virtual std::any visit(TypeAssignNode&) {
        return std::any();
    }

    virtual std::any visit(ValueAssignNode&) {
        return std::any();
    }

    virtual std::any visit(OutputNode&) {
        return std::any();
    }

    virtual std::any visit(ValNode&) {
        return std::any();
    }

    virtual std::any visit(ArgNode&) {
        return std::any();
    }

    virtual std::any visit(GlobalNode&) {
        return std::any();
    }
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
    void failure(const Input&, States&&...) noexcept {}

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

class ValNode: public BaseNode {
  public:
    int32_t value;

    explicit ValNode(int32_t value):
        BaseNode(BaseNode(), NodeTag::kVal), value(value) {}

    std::any accept(BaseVisitor* visitor) override {
        return visitor->visit(*this);
    }
};

class ArgNode: public BaseNode {
  public:
    size_t index;

    explicit ArgNode(size_t index):
        BaseNode(BaseNode(), NodeTag::kArg), index(index) {}

    std::any accept(BaseVisitor* visitor) override {
        return visitor->visit(*this);
    }
};

class GlobalNode: public BaseNode {
  public:
    size_t index;

    explicit GlobalNode(size_t index):
        BaseNode(BaseNode(), NodeTag::kGlobal), index(index) {}

    std::any accept(BaseVisitor* visitor) override {
        return visitor->visit(*this);
    }
};

/**
 * @brief Get node reference of casted n.
 * @tparam Node Target node type
 * @param n Node ptr to be casted
 * @return Node reference of casted n.
 */
template<typename Node>
Node& as(std::unique_ptr<BaseNode>& p) {
    return *static_cast<Node*>(p.get());
}

}  // namespace yacis::ast

#endif  // YACIS_AST_NODE_HPP_
