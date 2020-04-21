#ifndef YACIS_TYPE_HPP_
#define YACIS_TYPE_HPP_

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
    explicit Type(TypeTag type_tag);

    /**
     * @brief Construct function type from elements. Throw
     *        std::invalid_argument if the length is smaller than 2.
     */
    explicit Type(element_t ele);

    /**
     * @brief Merge function type at the end. If this is not a function type, do
     *        nothing.
     */
    void flatten();

    /**
     * @brief Apply a param, return remain type. Throw std::invalid_argument if
     *        not applicable. This function does not automatically flatten
     *        operands.
     */
    [[nodiscard]] Type apply(const Type& param) const;

    /**
     * @brief Compare whether two types are equal. This function does not
     *        automatically flatten operands.
     */
    friend bool operator==(const Type& lhs, const Type& rhs) noexcept;
    friend bool operator!=(const Type& lhs, const Type& rhs) noexcept;
};

}  // namespace yacis::ast

#endif  // YACIS_TYPE_HPP_
