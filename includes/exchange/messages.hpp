#pragma once
#include <cstdint>
#include <string>
#include "proto/exchange.pb.h"

struct NOS {
    uint8_t  order_id[16];
    std::string symbol;
    uint32_t price;
    uint32_t qty;
    hft::Side side;
    hft::OrdType type;
    uint64_t ts;
};

inline NOS from_proto(const hft::NewOrderSingle& p) {
    NOS o{};
    memcpy(o.order_id, p.order_id().data(), 16);
    o.symbol = p.symbol();
    o.price  = p.price();
    o.qty    = p.qty();
    o.side   = p.side();
    o.type   = p.ord_type();
    o.ts     = p.ts();
    return o;
}
