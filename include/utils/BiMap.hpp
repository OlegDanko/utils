#ifndef BIMAP_H
#define BIMAP_H

#include <unordered_map>

namespace utl_prf {

template<typename K, typename V>
class BiMap {
    std::unordered_map<K, V> key_to_val;
    std::unordered_map<V, K> val_to_key;
    template<typename From, typename To>
    To get(From f, std::unordered_map<From, To>& map) {
        if(auto it = map.find(f); it != map.end())
            return it->second;
        return 0;
    }

    template<typename From, typename To>
    void remove(From id, std::unordered_map<From, To>& map_a, std::unordered_map<To, From>& map_b) {
        if(auto it = map_a.find(id); it != map_a.end()) {
            if(auto it_ = map_b.find(it->second); it_ != map_b.end())
                map_b.erase(it_);
            map_a.erase(it);
        }
    }

public:
    K get_key(V id) {
        return get(id, val_to_key);
    }

    V get_val(K id) {
        return get(id, key_to_val);
    }

    void remove_key(K id) {
        remove(id, key_to_val, val_to_key);
    }

    void remove_val(V id) {
        remove(id, val_to_key, key_to_val);
    }

    void add(K k, V v) {
        remove_key(k);
        remove_val(v);
        key_to_val[k] = v;
        val_to_key[v] = k;
    }

    bool key_exists(K id) {
        return key_to_val.find(id) != key_to_val.end();
    }

    bool val_exists(V id) {
        return val_to_key.find(id) != val_to_key.end();
    }
};

using key_type = int;
using val_type = int;

#define BI_MAP_IMPL(class_name, key_name, key_type, val_name, val_type) \
    class class_name : BiMap<key_type, val_type> { \
        using Base = BiMap<key_type, val_type>;    \
    public:                                        \
        key_type get_##key_name(val_type id) {     \
            return Base::get_key(id);              \
        }                                          \
        val_type get_##val_name(key_type id) {     \
            return Base::get_val(id);              \
        }                                          \
        void remove_##key_name(key_type id) {      \
            Base::remove_key(id);                  \
        }                                          \
        void remove_##val_name(val_type id) {      \
            Base::remove_val(id);                  \
        }                                          \
        void add(key_type k, val_type v) {         \
            Base::add(k, v);                       \
        }                                          \
        bool key_name##_exists(key_type id) {      \
            return Base::key_exists(id);           \
        }                                          \
        bool val_name##_exists(val_type id) {      \
            return Base::val_exists(id);           \
        }                                          \
    };

}

#endif // BIMAP_H
