#pragma once
#include <memory>
#include <unordered_map>
#include <atomic>
#include "../utils.hpp"
#include "../Protected.hpp"

namespace utl_prf {

class OnDestroyedListener {
public:
    virtual void on_destroyed(size_t) = 0;
};


class TicketDispenser : OnDestroyedListener {
    std::atomic_size_t ticker_id_counter{1};
    std::function<void(size_t)> destroyed_callback_fn{nullptr};

    TicketDispenser(const TicketDispenser&) = delete;
    TicketDispenser(TicketDispenser&&) = delete;
    TicketDispenser& operator=(const TicketDispenser&) = delete;
    TicketDispenser& operator=(TicketDispenser&&) = delete;

public:
    TicketDispenser() = default;
    class Ticket {
        Ticket(OnDestroyedListener& listener, size_t id)
            : on_destroy_listener(listener)
            , id(id)
        {}
        OnDestroyedListener& on_destroy_listener;
        const size_t id;
    public:
        friend class TicketDispenser;
        ~Ticket() {
            on_destroy_listener.on_destroyed(id);
        }
    };
    using ticket_s_ptr_t = std::shared_ptr<const Ticket>;

    std::shared_ptr<const Ticket> create() {
        // can't make_shared because constructor is private :<
        return ticket_s_ptr_t(new Ticket(
                    *static_cast<OnDestroyedListener*>(this),
                    ticker_id_counter.fetch_add(1)
                    ));
    }
    bool check(const Ticket& t) {
        return static_cast<OnDestroyedListener*>(this) == &t.on_destroy_listener;
    }
    size_t get_id(const Ticket& t) {
        return t.id;
    }

    void set_destroyed_callback(std::function<void(size_t)> fn) {
        destroyed_callback_fn = fn;
    }
    void on_destroyed(size_t id) override {
        if(destroyed_callback_fn)
            destroyed_callback_fn(id);
    }
};

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
