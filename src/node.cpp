#include "yacis/node.hpp"

#include <utility>

namespace yacis::ast {

BaseNode::BaseNode(BaseNode&& base_node, NodeTag node_tag):
    tag(node_tag),
    children(std::move(base_node.children)),
    m_begin(base_node.m_begin),
    m_end(base_node.m_end){};

}  // namespace yacis::ast
