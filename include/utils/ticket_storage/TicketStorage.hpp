#pragma once
#include <memory>
#include <unordered_map>
#include "TicketDispenser.hpp"
#include "../Protected.hpp"

namespace utl_prf {

template<typename T>
class TicketStorage{
    using u_ptr_t = std::unique_ptr<T>;
    using item_ref_opt_t = std::optional<std::reference_wrapper<T>>;
    using ticket_s_ptr_t = TicketDispenser::ticket_s_ptr_t;

    TicketDispenser ticket_dispenser;
    utl_prf::Protected<std::unordered_map<size_t, u_ptr_t>> item_map;

    virtual void on_ticket_destroyed(size_t id) {
        with(auto locked_map = item_map.lock()) {
        IF_PRESENT(id, locked_map.get(), it)
                locked_map.get().erase(id);
        }
    }
public:
    // emphesises that item reference doesn't outlive the ticket
    class ItemWrapper {
    protected:
        TicketDispenser::ticket_s_ptr_t ticket;
        item_ref_opt_t item_ref_opt{std::nullopt};
        ItemWrapper(TicketDispenser::ticket_s_ptr_t t, item_ref_opt_t o = std::nullopt)
            : ticket(t)
            , item_ref_opt(o)
        {}
    public:
        friend class TicketStorage;
        operator bool() { return item_ref_opt.has_value(); }
        T& unwrap() {
            return item_ref_opt.value();
        }
    };

    TicketStorage() {
        ticket_dispenser.set_destroyed_callback(
                    [this](auto id){ on_ticket_destroyed(id); }
        );
    }

    auto add(u_ptr_t item) {
        auto ticket = ticket_dispenser.create();
        with(auto locked_map = item_map.lock()) {
            locked_map.get()[ticket_dispenser.get_id(*ticket)] = std::move(item);
        }
        return ticket;
    }

    ItemWrapper get(TicketDispenser::ticket_s_ptr_t t) {
        if(!ticket_dispenser.check(*t)) {
            return {t};
        }

        with(auto locked_map = item_map.lock()) {
            IF_PRESENT(ticket_dispenser.get_id(*t), locked_map.get(), it) {
                return {std::move(t), std::make_optional(std::ref(*it->second))};
            }
        }
        return {t};
    }
};

}
