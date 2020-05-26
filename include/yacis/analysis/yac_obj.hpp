#ifndef YACIS_ASM_GEN_YAC_OBJ_HPP_
#define YACIS_ASM_GEN_YAC_OBJ_HPP_

#include <memory>
#include <vector>

namespace yacis::analysis {

template<typename T>
class List: public std::enable_shared_from_this<List<T>> {
  public:
    const bool is_nil;
    const T val;
    const std::shared_ptr<const List<T>> tail;

    List(): is_nil(true), val(), tail() {}
    explicit List(T val, std::shared_ptr<const List<T>> tail = nullptr):
        is_nil(false), val(std::move(val)), tail(std::move(tail)) {}

    /**
     * @brief Return a shared_ptr of this object.
     */
    std::shared_ptr<const List<T>> get_ptr() const {
        return this->shared_from_this();
    }

    /**
     * @brief Index operator, starting from 0. If the index is greater than
     *        list length, return Nil node.
     */
    std::shared_ptr<const List<T>> index(size_t i) const {
        if (i && !is_nil)
            return tail->index(i - 1);
        else
            return get_ptr();
    }

    /**
     * @brief Enqueue an element.
     */
    std::shared_ptr<const List<T>> cons(T ele) const {
        return std::make_shared<const List<T>>(std::move(ele), get_ptr());
    }
};

class YacObj;

using ObjRc = std::shared_ptr<const YacObj>;
using Context = std::shared_ptr<const List<const ObjRc>>;

inline Context empty_context = std::make_shared<const List<const ObjRc>>();

class YacObj: public std::enable_shared_from_this<YacObj> {
  public:
    virtual ~YacObj() = default;

    /**
     * @brief Return a shared_ptr of this object.
     */
    ObjRc get_ptr() const {
        return this->shared_from_this();
    }

    virtual ObjRc eval(const Context&) const {
        return get_ptr();
    }
};

class YacVal: public YacObj {
  public:
    int32_t val;

    explicit YacVal(int32_t val): val(val) {}

    ObjRc eval(const Context&) const override {
        return get_ptr();
    }

    static const YacVal& from(const ObjRc& rc) {
        return *static_cast<const YacVal*>(&*rc);
    }
};

class YacArg: public YacObj {
  public:
    size_t index;

    explicit YacArg(size_t index): index(index) {}

    ObjRc eval(const Context& context) const override {
        return context->index(index)->val;
    }
};

class YacGlobal: public YacObj {
  public:
    std::vector<ObjRc>* global_vec;  // should be observer_ptr
    size_t index;

    YacGlobal(std::vector<ObjRc>* global_vec, size_t index):
        global_vec(global_vec), index(index) {}

    ObjRc eval(const Context&) const override {
        return (*global_vec)[index];  // lazy cuz recursions exist
    }
};

class YacFunc: public YacObj {
  public:
    const Context context;
    const size_t arg_num;
    const ObjRc body;

    YacFunc(size_t arg_num, ObjRc body):
        context(empty_context), arg_num(arg_num), body(std::move(body)) {}

    YacFunc(Context context, size_t arg_num, ObjRc body):
        context(std::move(context)), arg_num(arg_num), body(std::move(body)) {}

    ObjRc apply(const ObjRc& arg) const {
        return std::make_shared<const YacFunc>(
            context->cons(arg), arg_num - 1, body);
    }

    ObjRc eval(const Context&) const override {
        if (arg_num)
            return get_ptr();
        else
            return body->eval(context);
    }

    static const YacFunc& from(const ObjRc& rc) {
        return *static_cast<const YacFunc*>(&*rc);
    }
};

class YacAppl: public YacObj {
  public:
    const std::vector<ObjRc> ele;

    explicit YacAppl(std::vector<ObjRc> ele): ele(std::move(ele)) {}

    ObjRc eval(const Context& context) const override {
        auto it = ele.begin();
        auto func = (*it)->eval(context);
        for (++it; it != ele.end(); ++it)
            func = YacFunc::from(func).apply((*it)->eval(context));
        return YacFunc::from(func).eval(context);
    }
};

class YacCond: public YacObj {
  public:
    const ObjRc if_obj;
    const ObjRc then_obj;
    const ObjRc else_obj;

    YacCond(ObjRc if_obj, ObjRc then_obj, ObjRc else_obj):
        if_obj(std::move(if_obj)),
        then_obj(std::move(then_obj)),
        else_obj(std::move(else_obj)) {}

    ObjRc eval(const Context& context) const override {
        auto result = YacVal::from(if_obj->eval(context)).val;
        if (result)
            return then_obj->eval(context);
        else
            return else_obj->eval(context);
    }
};

}  // namespace yacis::analysis

#endif  // YACIS_ASM_GEN_YAC_OBJ_HPP_
