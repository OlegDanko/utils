#include "../../../include/utils/ticket_storage/TicketDispenser.hpp"

utl_prf::TicketDispenser::Ticket::Ticket(OnDestroyedListener &listener, size_t id)
    : on_destroy_listener(listener)
    , id(id)
{}

utl_prf::TicketDispenser::Ticket::~Ticket() {
    on_destroy_listener.on_destroyed(id);
}

std::shared_ptr<const utl_prf::TicketDispenser::Ticket> utl_prf::TicketDispenser::create() {
    // can't make_shared because constructor is private :<
    return ticket_s_ptr_t(new Ticket(
                              *static_cast<OnDestroyedListener*>(this),
                              ticker_id_counter.fetch_add(1)
                              ));
}

bool utl_prf::TicketDispenser::check(const Ticket &t) {
    return static_cast<OnDestroyedListener*>(this) == &t.on_destroy_listener;
}

size_t utl_prf::TicketDispenser::get_id(const Ticket &t) {
    return t.id;
}

void utl_prf::TicketDispenser::set_destroyed_callback(std::function<void (size_t)> fn) {
    destroyed_callback_fn = fn;
}

void utl_prf::TicketDispenser::on_destroyed(size_t id) {
    if(destroyed_callback_fn)
        destroyed_callback_fn(id);
}
