#ifndef YACIS_UTILITY_PRINT_TREE_HPP_
#define YACIS_UTILITY_PRINT_TREE_HPP_

namespace yacis::utility {

/**
 * @brief Convert given node tag to its name in c style string.
 */
inline constexpr const char* tag_to_cstr(ast::NodeTag tag) {
    switch (tag) {
    case ast::NodeTag::kRoot:
        return "Root";
    case ast::NodeTag::kIntLit:
        return "IntLit";
    case ast::NodeTag::kBoolLit:
        return "BoolLit";
    case ast::NodeTag::kCharLit:
        return "CharLit";
    case ast::NodeTag::kVarName:
        return "VarName";
    case ast::NodeTag::kTypeName:
        return "TypeName";
    case ast::NodeTag::kType:
        return "Type";
    case ast::NodeTag::kApplExpr:
        return "ApplExpr";
    case ast::NodeTag::kCondExpr:
        return "CondExpr";
    case ast::NodeTag::kLetExpr:
        return "LetExpr";
    case ast::NodeTag::kLambdaParam:
        return "LambdaParam";
    case ast::NodeTag::kLambdaExpr:
        return "LambdaExpr";
    case ast::NodeTag::kTypeAlias:
        return "TypeAlias";
    case ast::NodeTag::kTypeAssign:
        return "TypeAssign";
    case ast::NodeTag::kValueAssign:
        return "ValueAssign";
    case ast::NodeTag::kOutput:
        return "Output";
    case ast::NodeTag::kVal:
        return "Val";
    case ast::NodeTag::kArg:
        return "Arg";
    case ast::NodeTag::kGlobal:
        return "Global";
    }
}

inline void print_tree(std::unique_ptr<ast::BaseNode>& root, int indent = 0) {
    for (int i = 0; i < indent; ++i) std::cout.put(' ');
    std::cout << "- " << tag_to_cstr(root->tag) << std::endl;
    for (auto&& i : root->children) print_tree(i, indent + 2);
}

}  // namespace yacis::utility

#endif  // YACIS_UTILITY_PRINT_TREE_HPP_
