#ifndef YACIS_ANALYSIS_EVAL_HPP_
#define YACIS_ANALYSIS_EVAL_HPP_

#include <any>
#include <memory>
#include <utility>
#include <vector>

#include "yacis/analysis/type.hpp"
#include "yacis/analysis/yac_obj.hpp"
#include "yacis/ast/node.hpp"

namespace yacis::analysis {

namespace internal {

class YacNegate: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(-val);
    }
};

class YacAdd: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 + val2);
    }
};

class YacSub: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 - val2);
    }
};

class YacMul: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 * val2);
    }
};

class YacDiv: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 / val2);
    }
};

class YacMod: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 % val2);
    }
};

class YacEq: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 == val2);
    }
};

class YacNeq: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 != val2);
    }
};

class YacLt: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 < val2);
    }
};

class YacGt: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 > val2);
    }
};

class YacLeq: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 <= val2);
    }
};

class YacGeq: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 >= val2);
    }
};

class YacAnd: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 && val2);
    }
};

class YacOr: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val1 = YacVal::from(context->index(1)->val).val;
        auto val2 = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(val1 || val2);
    }
};

class YacNot: public YacObj {
  public:
    ObjRc eval(const Context& context) const override {
        auto val = YacVal::from(context->index(0)->val).val;
        return std::make_shared<YacVal>(!val);
    }
};

inline const std::vector<ObjRc> init_global_vec{
    std::make_shared<YacFunc>(1, std::make_shared<YacNegate>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacAdd>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacSub>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacMul>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacDiv>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacMod>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacEq>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacNeq>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacLt>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacGt>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacLeq>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacGeq>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacAnd>()),
    std::make_shared<YacFunc>(2, std::make_shared<YacOr>()),
    std::make_shared<YacFunc>(1, std::make_shared<YacNot>()),
};

class EvalVisitor: public ast::BaseVisitor {
  public:
    std::vector<ObjRc> global_vec = init_global_vec;
    std::vector<std::pair<int32_t, Type>> output;

    std::any call(std::unique_ptr<ast::BaseNode>& p) {
        return p->accept(this);
    }

    static std::any ret(const ObjRc& p) {
        // fuck any_cast
        return p;
    }

    std::any visit(ast::BaseNode& n) override {
        for (auto&& i : n.children) call(i);
        return std::any();
    }

    std::any visit(ast::ValNode& n) override {
        return ret(std::make_shared<YacVal>(n.value));
    }

    std::any visit(ast::ArgNode& n) override {
        return ret(std::make_shared<YacArg>(n.index));
    }

    std::any visit(ast::GlobalNode& n) override {
        return ret(std::make_shared<YacGlobal>(&global_vec, n.index));
    }

    std::any visit(ast::ApplExprNode& n) override {
        std::vector<ObjRc> ele;
        ele.reserve(n.children.size());
        for (auto&& i : n.children)
            ele.push_back(std::any_cast<const ObjRc>(call(i)));
        return ret(std::make_shared<YacAppl>(std::move(ele)));
    }

    std::any visit(ast::CondExprNode& n) override {
        return ret(std::make_shared<YacCond>(
            std::any_cast<const ObjRc>(call(n.children[0])),
            std::any_cast<const ObjRc>(call(n.children[1])),
            std::any_cast<const ObjRc>(call(n.children[2]))));
    }

    std::any visit(ast::LambdaExprNode& n) override {
        return ret(std::make_shared<YacFunc>(
            n.children.size() - 1,
            std::any_cast<const ObjRc>(call(n.children.back()))));
    }

    std::any visit(ast::ValueAssignNode& n) override {
        global_vec.push_back(std::any_cast<const ObjRc>(call(n.children[1]))
                                 ->eval(empty_context));
        return std::any();
    }

    std::any visit(ast::OutputNode& n) override {
        auto result = std::any_cast<const ObjRc>(call(n.children[0]));
        output.emplace_back(YacVal::from(result->eval(empty_context)).val,
                            n.info.type);
        return std::any();
    }
};

/**
 * @brief Analysis stage 3. Evaluate ast and output results.
 * @param root Root node of AST.
 */
inline std::vector<std::pair<int32_t, Type>>
eval(std::unique_ptr<ast::BaseNode>& root) {
    EvalVisitor visitor;
    visitor.call(root);
    return std::move(visitor.output);
}

}  // namespace internal

using internal::eval;

}  // namespace yacis::analysis

#endif  // YACIS_ANALYSIS_EVAL_HPP_
