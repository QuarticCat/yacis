#include "yacis/type.hpp"

#include <stdexcept>

namespace yacis::ast {

Type::Type(TypeTag type_tag): tag(type_tag), ele() {
    if (type_tag == TypeTag::kFunction)
        throw std::invalid_argument("type_tag can not be TypeTag::kFunction.");
};

Type::Type(element_t ele): tag(TypeTag::kFunction), ele(std::move(ele)) {
    if (this->ele.size() < 2)
        throw std::invalid_argument(
            "The length of comp should be greater than or equal to 2.");
}

Type Type::apply(const Type& param) const {
    auto param_size = param.ele.size();
    if (ele.size() <= param_size)
        throw std::invalid_argument("Not applicable.");
    for (element_t::size_type i = 0; i < param_size; ++i) {
        if (ele[i] != param.ele[i])
            throw std::invalid_argument("Not applicable.");
    }
    return Type(element_t(ele.begin() + param_size, ele.end()));
}

void Type::flatten() {
    if (tag != TypeTag::kFunction) return;
    for (;;) {
        auto back = ele.back();
        if (back.tag != TypeTag::kFunction) return;
        ele.pop_back();
        ele.reserve(ele.size() + back.ele.size());
        std::move(back.ele.begin(), back.ele.end(), std::back_inserter(ele));
    }
}

bool operator==(const Type& lhs, const Type& rhs) noexcept {
    if (lhs.tag != rhs.tag) return false;
    if (lhs.tag == TypeTag::kFunction) return lhs.ele == rhs.ele;
    return true;
}

bool operator!=(const Type& lhs, const Type& rhs) noexcept {
    return !(lhs == rhs);
}

}  // namespace yacis::ast
