#ifndef YACIS_SYMBOL_TABLE_HPP_
#define YACIS_SYMBOL_TABLE_HPP_

#include <map>
#include <memory>
#include <string>

namespace yacis::ast {

template<typename Key, typename Value>
class ChainMap: std::enable_shared_from_this<ChainMap<Key, Value>> {
  public:
    std::shared_ptr<ChainMap<Key, Value>> parent;
    std::map<Key, Value> map;

    ChainMap() = default;
    ChainMap(const ChainMap<Key, Value>&) = default;
    ChainMap(ChainMap<Key, Value>&&) noexcept = default;
    ChainMap<Key, Value>& operator=(const ChainMap<Key, Value>&) = default;
    ChainMap<Key, Value>& operator=(ChainMap<Key, Value>&&) noexcept = default;

    /**
     * @brief Construct chain map with given parent and empty map.
     */
    explicit ChainMap(std::shared_ptr<ChainMap<Key, Value>> parent):
        parent(parent), map(){};

    /**
     * @brief Return a shared_ptr of this object.
     */
    std::shared_ptr<ChainMap<Key, Value>> get_ptr() {
        return this->shared_from_this();
    }

    /**
     * @brief Return a new child of this object.
     */
    std::shared_ptr<ChainMap<Key, Value>> new_child() {
        return std::make_shared<ChainMap<Key, Value>>(get_ptr());
    }

    /**
     * @brief Return whether this object is on the top of its chain.
     */
    bool is_top() {
        return parent == nullptr;
    }

    /**
     * @brief Return whether the chain contains given key.
     */
    bool contains(const Key& key) {
        if (is_top())
            return map.count(key) > 0;
        else if (map.count(key))
            return true;
        else
            return parent->contains(key);
    }

    /**
     * @brief Return closest value of given key in the chain if the key exists.
     *        Otherwise insert the key into the map of this object.
     */
    Value& operator[](const Key& key) {
        if (contains(key))
            return index_helper(key);
        else
            return map[key];
    }

  private:
    /**
     * @brief A helper function for operator[] .
     */
    Value& index_helper(const Key& key) {
        if (map.count(key))
            return map[key];
        else
            return parent->index_helper(key);
    }
};

template<typename Value>
using SymbolTable = ChainMap<std::string, Value>;

}  // namespace yacis::ast

#endif  // YACIS_SYMBOL_TABLE_HPP_
