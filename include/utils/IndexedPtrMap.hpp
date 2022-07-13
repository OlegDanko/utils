#pragma once

#include "IdInc.hpp"

namespace utl_prf {

template<typename ID_t, typename Obj_t>
struct IndexedPtrMap {
    IdInc<ID_t> id_inc;
    std::unordered_map<ID_t, std::unique_ptr<Obj_t>> map;

    ID_t gen_id(ID_t id = 0) {
        if(id == 0) return id_inc.get_next();

        id_inc.reserve(id);
        return id;
    }

    ID_t insert(std::unique_ptr<Obj_t> obj, ID_t id = 0) {
        id = gen_id(id);
        map[id] = std::move(obj);
        return id;
    }

    template<typename Obj = Obj_t>
    std::pair<ID_t, Obj_t&> gen(ID_t id = 0) {
        id = gen_id(id);
        map[id] = std::make_unique<Obj>();
        return {id, *map[id]};
    }

    void remove(ID_t id) {
        IF_PRESENT(id, map, it)
                map.erase(it);
        id_inc.restore(id);
    }

    Obj_t* get(ID_t id) {
        IF_PRESENT(id, map, it)
                return it->second.get();
        return nullptr;
    }
};

}
