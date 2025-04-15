#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "../includes/logger.h"
#include "../includes/concurrentqueue.h"
#include "../includes/plf_hive.h"
#include "../includes/robin_hood.h"
#include "../includes/types.h"

#include "../src/orderbook.h"

logger* g_test_logger = new logger("../logs/test_orderbook.log");

order_t make_order(
   uint64_t timestamp,
   const char* order_id_str,
   const char* ticker_str,
   order_kind kind,
   order_side side,
   order_status status,
   uint32_t price,
   size_t qty,
   bool post_only)
{
   return order_t(
      timestamp,
      order_id_str,
      ticker_str,
      kind,
      side,
      status,
      price,
      qty,
      post_only
   );
}

TEST_CASE("Orderbook: add() basic tests", "[orderbook]")
{
    orderbook ob(g_test_logger);

    // 4-byte ticker "ABCD"
    char TICKER_ABCD[4] = { 'A','B','C','D' };

    // 16-byte ID #1
    char ID1[16] = {
        'O','R','D','0','0','0','0','0','0','0','0','0','0','0','0','1'
    };
    order_t o1 = make_order(
        1000000ULL,
        ID1,
        TICKER_ABCD,
        order_kind::LMT,
        order_side::BUY,
        order_status::NEW,
        100,
        10,
        false
    );
    REQUIRE(ob.add(o1) == order_result::SUCCESS);

    order_id_key key1;
    std::memcpy(key1.order_id, ID1, 16);
    REQUIRE(ob.contains(key1) == true);

    auto bb = ob.best_bid();
    REQUIRE(bb.has_value());
    REQUIRE(bb.value() == 100);

    // 16-byte ID #2
    char ID2[16] = {
        'O','R','D','0','0','0','0','0','0','0','0','0','0','0','0','2'
    };
    order_t o2 = make_order(
        1000010ULL,
        ID2,
        TICKER_ABCD,
        order_kind::LMT,
        order_side::SELL,
        order_status::NEW,
        105,
        5,
        false
    );
    REQUIRE(ob.add(o2) == order_result::SUCCESS);

    order_id_key key2;
    std::memcpy(key2.order_id, ID2, 16);
    REQUIRE(ob.contains(key2) == true);

    auto ba = ob.best_ask();
    REQUIRE(ba.has_value());
    REQUIRE(ba.value() == 105);
}