#pragma once

#include "../utils.hpp"
#include <atomic>

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
        Ticket(OnDestroyedListener& listener, size_t id);
        OnDestroyedListener& on_destroy_listener;
        const size_t id;
    public:
        friend class TicketDispenser;
        ~Ticket();
    };
    using ticket_s_ptr_t = std::shared_ptr<const Ticket>;

    std::shared_ptr<const Ticket> create();
    bool check(const Ticket& t);
    size_t get_id(const Ticket& t);

    void set_destroyed_callback(std::function<void(size_t)> fn);
    void on_destroyed(size_t id) override;
};

}
