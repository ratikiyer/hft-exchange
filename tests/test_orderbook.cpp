#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "../includes/logger.h"
#include "../includes/concurrentqueue.h"
#include "../includes/plf_hive.h"
#include "../includes/robin_hood.h"
#include "../includes/types.h"

#include "../src/orderbook.h"

/*
  Global logger pointer used across tests.
  In a real-world scenario, you might want
  a separate logger per test or at least
  per test suite. For simplicity here, 
  we'll share one logger.
*/
logger* g_test_logger = new logger("../logs/test_orderbook.log");

/**
 * Helper function to create an order_t struct.
 */
order_t make_order(
    uint64_t timestamp,
    const char* order_id_str,
    const char* ticker_str,
    order_kind kind,
    order_side side,
    order_status status,
    uint32_t price,
    size_t qty,
    bool post_only
)
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

/**
 * Helper function to create an order_id_key struct 
 * from a 16-byte character array (e.g. ID1[16]).
 */
order_id_key make_key(const char (&id)[16])
{
    order_id_key k;
    std::memcpy(k.order_id, id, ORDER_ID_LEN);
    return k;
}

TEST_CASE("Orderbook: add() basic tests", "[orderbook][add]")
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

    auto key1 = make_key(ID1);
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

    auto key2 = make_key(ID2);
    REQUIRE(ob.contains(key2) == true);

    auto ba = ob.best_ask();
    REQUIRE(ba.has_value());
    REQUIRE(ba.value() == 105);
}

TEST_CASE("Orderbook: add() duplicate ID", "[orderbook][add]")
{
    orderbook ob(g_test_logger);

    char TICKER_XYZ[4] = { 'X','Y','Z',' ' };
    char ID[16] = {
        'D','U','P','L','I','D','0','0','0','0','0','0','0','0','0','1'
    };

    order_t o1 = make_order(
        123456ULL,
        ID,
        TICKER_XYZ,
        order_kind::LMT,
        order_side::BUY,
        order_status::NEW,
        50,
        10,
        false
    );
    REQUIRE(ob.add(o1) == order_result::SUCCESS);

    // Try adding again with the same ID
    order_t o2 = make_order(
        123457ULL,
        ID,  // same ID
        TICKER_XYZ,
        order_kind::LMT,
        order_side::BUY,
        order_status::NEW,
        51,
        20,
        false
    );
    REQUIRE(ob.add(o2) == order_result::DUPLICATE_ID);
}

TEST_CASE("Orderbook: add() invalid side", "[orderbook][add]")
{
    orderbook ob(g_test_logger);

    char TICKER_ABC[4] = { 'A','B','C',' ' };
    char ID[16] = {
        'I','N','V','S','I','D','E','0','0','0','0','0','0','0','0','1'
    };

    // side=2 is invalid (only 0=BUY,1=SELL are valid)
    order_t invalid_side_order = {};
    std::memcpy(invalid_side_order.order_id, ID, 16);
    std::memcpy(invalid_side_order.ticker, TICKER_ABC, 4);
    invalid_side_order.timestamp = 111111ULL;
    invalid_side_order.price = 100;
    invalid_side_order.qty = 10;
    invalid_side_order.side = -1;  // invalid
    REQUIRE(ob.add(invalid_side_order) == order_result::INVALID_SIDE);
}

TEST_CASE("Orderbook: add() invalid price", "[orderbook][add]")
{
    orderbook ob(g_test_logger);

    char TICKER_ABC[4] = { 'A','B','C',' ' };
    char ID[16] = {
        'I','N','V','P','R','I','C','E','0','0','0','0','0','0','0','1'
    };

    // Price > MAX_PRICE (which is 20000)
    order_t invalid_price_order = make_order(
        222222ULL,
        ID,
        TICKER_ABC,
        order_kind::LMT,
        order_side::BUY,
        order_status::NEW,
        30000,  // invalid
        10,
        false
    );

    REQUIRE(ob.add(invalid_price_order) == order_result::INVALID_PRICE);
}

TEST_CASE("Orderbook: cancel() basic", "[orderbook][cancel]")
{
    orderbook ob(g_test_logger);

    char TICKER_ABC[4] = { 'A','B','C',' ' };

    char ID1[16] = {
        'C','N','C','L','0','0','0','0','0','0','0','0','0','0','0','1'
    };
    auto o1 = make_order(
        100ULL,
        ID1,
        TICKER_ABC,
        order_kind::LMT,
        order_side::SELL,
        order_status::NEW,
        150,
        25,
        false
    );

    REQUIRE(ob.add(o1) == order_result::SUCCESS);

    auto key1 = make_key(ID1);
    REQUIRE(ob.contains(key1) == true);

    REQUIRE(ob.cancel(key1) == order_result::SUCCESS);
    REQUIRE(ob.contains(key1) == false);

    // best_ask should now be empty
    REQUIRE_FALSE(ob.best_ask().has_value());
    // best_bid also empty
    REQUIRE_FALSE(ob.best_bid().has_value());
}

TEST_CASE("Orderbook: cancel() order not found", "[orderbook][cancel]")
{
    orderbook ob(g_test_logger);

    char ID[16] = {
        'N','O','T','_','F','O','U','N','D','0','0','0','0','0','0','0'
    };
    auto key = make_key(ID);

    // Canceling a non-existent order should fail
    REQUIRE(ob.cancel(key) == order_result::ORDER_NOT_FOUND);
}

TEST_CASE("Orderbook: modify() same price", "[orderbook][modify]")
{
    orderbook ob(g_test_logger);

    // Create a BUY order
    char TICKER_ABC[4] = { 'A','B','C',' ' };
    char ID1[16] = {
        'M','O','D','-','S','A','M','E','-','P','R','I','C','E','-','1'
    };

    order_t o1 = make_order(
        999ULL,
        ID1,
        TICKER_ABC,
        order_kind::LMT,
        order_side::BUY,
        order_status::NEW,
        200,
        10,
        false
    );
    REQUIRE(ob.add(o1) == order_result::SUCCESS);

    // Modify the existing order with the same price but different qty
    order_t modified_o1 = o1; // copy
    modified_o1.qty = 20;
    modified_o1.timestamp = 1000ULL; // new timestamp

    auto key1 = make_key(ID1);
    REQUIRE(ob.modify(key1, modified_o1) == order_result::SUCCESS);

    // best_bid should remain 200
    auto bb = ob.best_bid();
    REQUIRE(bb.has_value());
    REQUIRE(bb.value() == 200);

    // Confirm that the order is still there under the same ID
    REQUIRE(ob.contains(key1) == true);
}

TEST_CASE("Orderbook: modify() change price", "[orderbook][modify]")
{
    orderbook ob(g_test_logger);

    char TICKER_ABC[4] = { 'A','B','C',' ' };
    char ID1[16] = {
        'M','O','D','C','H','G','P','R','I','C','E','0','0','0','0','1'
    };

    // Add BUY at price=150
    order_t o1 = make_order(
        500ULL,
        ID1,
        TICKER_ABC,
        order_kind::LMT,
        order_side::BUY,
        order_status::NEW,
        150,
        10,
        false
    );
    REQUIRE(ob.add(o1) == order_result::SUCCESS);

    // Modify it to price=180
    order_t modified_o1 = o1;
    modified_o1.price = 180;
    modified_o1.qty   = 15;
    modified_o1.timestamp = 510ULL;

    auto key1 = make_key(ID1);
    REQUIRE(ob.modify(key1, modified_o1) == order_result::SUCCESS);

    // The best bid should now be 180
    auto bb = ob.best_bid();
    REQUIRE(bb.has_value());
    REQUIRE(bb.value() == 180);

    // We have no ask orders in the book, so best_ask() is std::nullopt
    REQUIRE_FALSE(ob.best_ask().has_value());
}

TEST_CASE("Orderbook: modify() nonexistent order", "[orderbook][modify]")
{
    orderbook ob(g_test_logger);

    char ID[16] = { 
        'N','O','N','E','X','I','S','T','0','0','0','0','0','0','0','1' 
    };

    order_t some_order;
    std::memcpy(some_order.order_id, ID, 16);
    some_order.price = 100;
    some_order.qty   = 10;

    auto key = make_key(ID);
    // Attempt to modify an order not in the book
    REQUIRE(ob.modify(key, some_order) == order_result::ORDER_NOT_FOUND);
}

TEST_CASE("Orderbook: execute() basic match", "[orderbook][execute]")
{
    orderbook ob(g_test_logger);

    /*
       Create a simple scenario:
         - BUY:  Price=100, Qty=10
         - SELL: Price=90,  Qty=5
       We expect immediate match since best_bid_price_ >= best_ask_price_.
       The match should partially fill the BUY if SELL has less quantity.
    */

    char ID_BUY[16] = {
        'E','X','E','C','-','B','A','S','I','C','-','B','U','Y','-','1'
    };
    order_t buy_o = make_order(
        1000ULL,
        ID_BUY,
        "ABCD",
        order_kind::LMT,
        order_side::BUY,
        order_status::NEW,
        100,   // price
        10,    // qty
        false
    );
    REQUIRE(ob.add(buy_o) == order_result::SUCCESS);

    // SELL at 90, qty=5
    char ID_SELL[16] = {
        'E','X','E','C','-','B','A','S','I','C','-','S','E','L','L','1'
    };
    order_t sell_o = make_order(
        1001ULL,
        ID_SELL,
        "ABCD",
        order_kind::LMT,
        order_side::SELL,
        order_status::NEW,
        90,   // price
        5,    // qty
        false
    );
    REQUIRE(ob.add(sell_o) == order_result::SUCCESS);

    // best_bid = 100, best_ask = 90 => match possible
    ob.execute();

    // The SELL order should fully fill (qty=5). The BUY remains partially filled (5 left).
    auto key_sell = make_key(ID_SELL);
    REQUIRE(ob.contains(key_sell) == false); // fully filled => removed

    auto key_buy = make_key(ID_BUY);
    REQUIRE(ob.contains(key_buy) == true);   // partially filled => remains

    // best_ask should now be empty
    REQUIRE_FALSE(ob.best_ask().has_value());

    // best_bid should still be 100 (the partial remainder)
    REQUIRE(ob.best_bid().has_value());
    REQUIRE(ob.best_bid().value() == 100);
}

TEST_CASE("Orderbook: execute() multiple matches", "[orderbook][execute]")
{
    /*
      Scenario:
        - 2 BUY orders:  (price=100, qty=5),  (price=95, qty=10)
        - 2 SELL orders: (price=90,  qty=6),  (price=85, qty=10)

      best_bid=100, best_ask=85 => everything will keep matching until
      no more cross can occur.
    */
    orderbook ob(g_test_logger);

    // BUY #1
    char IDB1[16] = { 'M','U','L','T','I','-','B','U','Y','-','0','0','0','0','0','1' };
    auto b1 = make_order(
        1000ULL, 
        IDB1, 
        "ABCD", 
        order_kind::LMT, 
        order_side::BUY, 
        order_status::NEW, 
        100, 
        5, 
        false
    );
    REQUIRE(ob.add(b1) == order_result::SUCCESS);

    // BUY #2
    char IDB2[16] = { 'M','U','L','T','I','-','B','U','Y','-','0','0','0','0','0','2' };
    auto b2 = make_order(
        1001ULL, 
        IDB2, 
        "ABCD", 
        order_kind::LMT, 
        order_side::BUY, 
        order_status::NEW, 
        95, 
        10, 
        false
    );
    REQUIRE(ob.add(b2) == order_result::SUCCESS);

    // SELL #1
    char IDS1[16] = { 'M','U','L','T','I','-','S','E','L','L','-','0','0','0','0','1' };
    auto s1 = make_order(
        1010ULL, 
        IDS1, 
        "ABCD", 
        order_kind::LMT, 
        order_side::SELL, 
        order_status::NEW, 
        90, 
        6, 
        false
    );
    REQUIRE(ob.add(s1) == order_result::SUCCESS);

    // SELL #2
    char IDS2[16] = { 'M','U','L','T','I','-','S','E','L','L','-','0','0','0','0','2' };
    auto s2 = make_order(
        1011ULL, 
        IDS2, 
        "ABCD", 
        order_kind::LMT, 
        order_side::SELL, 
        order_status::NEW, 
        85, 
        10, 
        false
    );
    REQUIRE(ob.add(s2) == order_result::SUCCESS);

    // Confirm best_bid=100, best_ask=85
    REQUIRE(ob.best_bid().has_value());
    REQUIRE(ob.best_bid().value() == 100);

    REQUIRE(ob.best_ask().has_value());
    REQUIRE(ob.best_ask().value() == 85);

    // Execute all matches
    ob.execute();

    // After execution:
    //  - b1(100@5), b2(95@10), s2(85@10), s1(90@6)
    //  Reasoning:
    //    b1 -> fully fills vs s2 partial => s2 leftover 5
    //    b2 -> partial fill with remaining s2 => leftover b2=5, s2 fully filled
    //    b2 -> then tries s1 => leftover s1=1, b2 fully filled
    //    => b1 gone, b2 gone, s2 gone, s1 remains with qty=1
    auto kb1  = make_key(IDB1);
    auto kb2  = make_key(IDB2);
    auto ks1  = make_key(IDS1);
    auto ks2  = make_key(IDS2);

    REQUIRE_FALSE(ob.contains(kb1)); // fully executed
    REQUIRE_FALSE(ob.contains(kb2)); // fully executed
    REQUIRE_FALSE(ob.contains(ks2)); // fully executed
    REQUIRE(ob.contains(ks1));       // partial fill => remains

    // best_ask should now be 90 (from s1 leftover)
    REQUIRE(ob.best_ask().has_value());
    REQUIRE(ob.best_ask().value() == 90);

    // best_bid should not exist (no more buys)
    REQUIRE_FALSE(ob.best_bid().has_value());
}

TEST_CASE("Orderbook: empty best_bid / best_ask", "[orderbook]")
{
    orderbook ob(g_test_logger);

    REQUIRE_FALSE(ob.best_bid().has_value());
    REQUIRE_FALSE(ob.best_ask().has_value());

    // Add a single BUY
    char ID_B[16] = { 'E','M','P','T','Y','-','B','I','D','-','T','E','S','T','0','1' };
    auto b = make_order(
        123ULL,
        ID_B,
        "EFGH",
        order_kind::LMT,
        order_side::BUY,
        order_status::NEW,
        500,
        10,
        false
    );
    REQUIRE(ob.add(b) == order_result::SUCCESS);

    REQUIRE(ob.best_bid().has_value());
    REQUIRE(ob.best_bid().value() == 500);
    REQUIRE_FALSE(ob.best_ask().has_value()); // no SELL

    // Cancel the BUY
    auto kb = make_key(ID_B);
    REQUIRE(ob.cancel(kb) == order_result::SUCCESS);

    REQUIRE_FALSE(ob.best_bid().has_value());
    REQUIRE_FALSE(ob.best_ask().has_value());
}

TEST_CASE("Orderbook: boundary test at max price", "[orderbook][boundary]")
{
    orderbook ob(g_test_logger);

    /*
      Since MAX_PRICE = 20000, let's place a BUY or SELL exactly at 20000
      and confirm it succeeds. Then see if a SELL with price=20001 fails.
    */

    char ID_BMax[16] = { 'B','M','A','X','P','R','I','C','E','0','0','0','0','0','0','B' };
    char ID_SInv[16] = { 'S','I','N','V','P','R','I','C','E','0','0','0','0','0','0','S' };

    // Valid boundary
    order_t bmax = make_order(
        100ULL,
        ID_BMax,
        "ZZZZ",
        order_kind::LMT,
        order_side::BUY,
        order_status::NEW,
        20000,
        10,
        false
    );
    REQUIRE(ob.add(bmax) == order_result::SUCCESS);

    // Attempt SELL at price=20001 (invalid)
    order_t sinv = make_order(
        101ULL,
        ID_SInv,
        "ZZZZ",
        order_kind::LMT,
        order_side::SELL,
        order_status::NEW,
        20001,  // invalid
        5,
        false
    );
    REQUIRE(ob.add(sinv) == order_result::INVALID_PRICE);
}
