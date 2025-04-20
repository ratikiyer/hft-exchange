#include "exchange/engine.hpp"

void MatchingEngine::fill_exec(const NOS& src, uint32_t px, uint32_t qty,
                               bool reject, std::string text)
{
    hft::ExecutionReport rpt;
    rpt.set_order_id(src.symbol() + std::string((char*)src.order_id,16));
    rpt.set_fill_px(px);
    rpt.set_fill_qty(qty);
    rpt.set_leaves_qty(src.qty - qty);
    rpt.set_reject(reject);
    rpt.set_text(text);
    out_q_.enqueue(std::move(rpt));
}

void MatchingEngine::on_nos(const NOS& n)
{
    orderbook& ob = books_[n.symbol];
    order_t ord{};
    memcpy(ord.order_id, n.order_id, 16);
    ord.timestamp = n.ts;                // fine for now
    ord.price = n.price;
    ord.qty   = n.qty;
    ord.side  = static_cast<uint8_t>(n.side);

    switch (n.type) {
        case hft::LIMIT:  ob.add(ord);  break;
        case hft::MARKET: // convert px to current best
            ord.price = (n.side == hft::BUY)
                       ? ob.best_ask().value_or(MAX_PRICE)
                       : ob.best_bid().value_or(0);
            ob.add(ord);
            break;
        case hft::IOC:    ob.add(ord);  break;
    }
    ob.execute();
    fill_exec(n, ord.price, n.qty - ob.contains({ord.order_id}) ? 0 : n.qty, false);
}
