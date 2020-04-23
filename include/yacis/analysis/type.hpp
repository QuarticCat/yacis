#ifndef YACIS_ANALYSIS_TYPE_HPP_
#define YACIS_ANALYSIS_TYPE_HPP_

#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <vector>

namespace yacis::analysis {

enum class TypeTag { kInt, kBool, kChar, kFunction, kUndefined };

class Type {
  public:
    using element_t = std::vector<Type>;

    TypeTag tag = TypeTag::kUndefined;
    element_t ele;

    Type() = default;
    Type(const Type& other) = default;
    Type(Type&& other) = default;
    Type& operator=(const Type& other) = default;
    Type& operator=(Type&& other) = default;

    /**
     * @brief Construct non-function type from type tag. Throw
     *        std::invalid_argument if type_tag == TypeTag::kFunction.
     */
    explicit Type(TypeTag type_tag): tag(type_tag), ele() {
        if (type_tag == TypeTag::kFunction)
            throw std::invalid_argument(
                "type_tag can not be TypeTag::kFunction.");
    };

    /**
     * @brief Construct function type from elements. Throw
     *        std::invalid_argument if the length is smaller than 2.
     */
    explicit Type(element_t ele): tag(TypeTag::kFunction), ele(std::move(ele)) {
        if (this->ele.size() < 2)
            throw std::invalid_argument(
                "The length of ele should be greater than or equal to 2.");
    }

    /**
     * @brief Construct function type from initializer_list. Throw
     *        std::invalid_argument if the length is smaller than 2.
     */
    Type(std::initializer_list<Type> ele): Type(element_t(ele)) {}

    /**
     * @brief Merge function type at the end. If this is not a function type, do
     *        nothing.
     */
    void flatten() noexcept {
        if (tag != TypeTag::kFunction) return;
        while (true) {
            auto back = ele.back();
            if (back.tag != TypeTag::kFunction) return;
            ele.pop_back();
            ele.reserve(ele.size() + back.ele.size());
            std::move(
                back.ele.begin(), back.ele.end(), std::back_inserter(ele));
        }
    }

    /**
     * @brief Apply a param, make this be the remain type. Throw
     *        std::invalid_argument if not applicable. This function does not
     *        automatically flatten operands.
     */
    void apply(const Type& param) {
        if (tag != TypeTag::kFunction || ele[0]!=param)
            throw std::invalid_argument("Not applicable.");
        if (ele.size() == 2)
            *this = ele.back();
        else
            ele.erase(ele.begin());
    }

    /**
     * @brief Compare whether two types are equal. This function does not
     *        automatically flatten operands.
     */
    friend bool operator==(const Type& lhs, const Type& rhs) noexcept {
        if (lhs.tag != rhs.tag) return false;
        if (lhs.tag == TypeTag::kFunction) return lhs.ele == rhs.ele;
        return true;
    }

    friend bool operator!=(const Type& lhs, const Type& rhs) noexcept {
        return !(lhs == rhs);
    }
};

inline const Type t_int(TypeTag::kInt);
inline const Type t_bool(TypeTag::kBool);
inline const Type t_char(TypeTag::kChar);

}  // namespace yacis::analysis

#endif  // YACIS_ANALYSIS_TYPE_HPP_
