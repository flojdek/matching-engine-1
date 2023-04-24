#include <chrono>
#include <algorithm>
#include <deque>

#include "gtest/gtest.h"
#include "../Markets.hpp"
#include "../Solution.hpp"
#include "../Trade.hpp"

using namespace std::chrono;

TEST(MatchingEngineTest, Match0_lonesome_bid) { 
    Solution s;

    auto bid1 = Order("b1", Order::BID, "BTCUSD", 1, 10);
    s.execute(bid1);
    ASSERT_TRUE(s.trades.empty());

    OrderBook::OrdersT bids, asks;
    std::tie(bids, asks) = s.markets.getBidsAndAsks("BTCUSD");
    ASSERT_EQ(bids.size(), 1);
    ASSERT_EQ(bids.at(0), bid1);
    ASSERT_EQ(asks.size(), 0);
}

TEST(MatchingEngineTest, Match0_lonesome_ask) { 
    Solution s;

    auto ask1 = Order("a1", Order::ASK, "BTCUSD", 1, 10);
    s.execute(ask1);
    ASSERT_EQ(s.trades.size(), 0);

    OrderBook::OrdersT bids, asks;
    std::tie(bids, asks) = s.markets.getBidsAndAsks("BTCUSD");
    ASSERT_EQ(bids.size(), 0);
    ASSERT_EQ(asks.size(), 1);
    ASSERT_EQ(asks.at(0), ask1);
}

TEST(MatchingEngineTest, Match1_incoming_ff_standing_ff) { 
    Solution s;

    auto bid1 = Order("b1", Order::BID, "BTCUSD", 1, 10);
    s.execute(bid1);
    ASSERT_TRUE(s.trades.empty());

    auto ask1 = Order("a1", Order::ASK, "BTCUSD", 1, 10);
    s.execute(ask1);
    ASSERT_EQ(s.trades.size(), 1);
    ASSERT_EQ(s.trades.at(0), Trade("BTCUSD", "a1", "b1", 1, 10));

    OrderBook::OrdersT bids, asks;
    std::tie(bids, asks) = s.markets.getBidsAndAsks("BTCUSD");
    ASSERT_EQ(bids.size(), 0);
    ASSERT_EQ(asks.size(), 0);
}

TEST(MatchingEngineTest, Match1_incoming_pf_standing_ff) { 
    Solution s;

    auto bid1 = Order("b1", Order::BID, "BTCUSD", 1, 10);
    s.execute(bid1);
    ASSERT_TRUE(s.trades.empty());

    auto ask1 = Order("a1", Order::ASK, "BTCUSD", 2, 10);
    s.execute(ask1);
    ASSERT_EQ(s.trades.size(), 1);
    ASSERT_EQ(s.trades.at(0), Trade("BTCUSD", "a1", "b1", 1, 10));

    OrderBook::OrdersT bids, asks;
    std::tie(bids, asks) = s.markets.getBidsAndAsks("BTCUSD");
    ASSERT_EQ(bids.size(), 0);
    ASSERT_EQ(asks.size(), 1);
    ASSERT_EQ(asks.at(0), Order("a1", Order::ASK, "BTCUSD", 1, 10));
}

TEST(MatchingEngineTest, Match1_incoming_ff_standing_pf) { 
    Solution s;

    auto bid1 = Order("b1", Order::BID, "BTCUSD", 2, 10);
    s.execute(bid1);
    ASSERT_TRUE(s.trades.empty());

    auto ask1 = Order("a1", Order::ASK, "BTCUSD", 1, 10);
    s.execute(ask1);
    ASSERT_EQ(s.trades.size(), 1);
    ASSERT_EQ(s.trades.at(0), Trade("BTCUSD", "a1", "b1", 1, 10));

    OrderBook::OrdersT bids, asks;
    std::tie(bids, asks) = s.markets.getBidsAndAsks("BTCUSD");
    ASSERT_EQ(bids.size(), 1);
    ASSERT_EQ(bids.at(0), Order("b1", Order::BID, "BTCUSD", 1, 10));
    ASSERT_EQ(asks.size(), 0);
}

TEST(MatchingEngineTest, OrderByArrival1) { 
    Solution s;

    auto o1 = Order("12345", Order::BID, "BTCUSD", 5, 10000);
    auto o2 = Order("zod42", Order::ASK, "BTCUSD", 2, 10001);
    auto o3 = Order("13471", Order::BID, "BTCUSD", 6, 9971);
    auto o4 = Order("11431", Order::BID, "ETHUSD", 9, 175);
    auto o5 = Order("abe14", Order::ASK, "BTCUSD", 7, 9800);
    auto o6 = Order("plu401", Order::ASK, "ETHUSD", 5, 170);
    auto o7 = Order("45691", Order::BID, "ETHUSD", 3, 180);

    s.execute(o1);
    s.execute(o2);
    s.execute(o3);
    s.execute(o4);
    s.execute(o5);
    s.execute(o6);
    s.execute(o7);

    ASSERT_EQ(s.trades.size(), 3);
    ASSERT_EQ(s.trades.at(0), Trade("BTCUSD", "abe14", "12345", 5, 10000));
    ASSERT_EQ(s.trades.at(1), Trade("BTCUSD", "abe14", "13471", 2, 9971));
    ASSERT_EQ(s.trades.at(2), Trade("ETHUSD", "plu401", "11431", 5, 175));

    std::deque<Order> openByArrival = s.getOpenOrdersByArrival();
    ASSERT_EQ(openByArrival.size(), 4);
    ASSERT_EQ(openByArrival.at(0), Order("zod42", Order::ASK, "BTCUSD", 2, 10001));
    ASSERT_EQ(openByArrival.at(1), Order("13471", Order::BID, "BTCUSD", 4, 9971));
    ASSERT_EQ(openByArrival.at(2), Order("11431", Order::BID, "ETHUSD", 4, 175));
    ASSERT_EQ(openByArrival.at(3), Order("45691", Order::BID, "ETHUSD", 3, 180));

    OrderBook::OrdersT btcbids, btcasks;
    std::tie(btcbids, btcasks) = s.markets.getBidsAndAsks("BTCUSD");
    ASSERT_EQ(btcasks.size(), 1);
    ASSERT_EQ(btcasks.at(0), Order("zod42", Order::ASK, "BTCUSD", 2, 10001));
    ASSERT_EQ(btcbids.size(), 1);
    ASSERT_EQ(btcbids.at(0), Order("13471", Order::BID, "BTCUSD", 4, 9971));

    OrderBook::OrdersT ethbids, ethasks;
    std::tie(ethbids, ethasks) = s.markets.getBidsAndAsks("ETHUSD");
    ASSERT_EQ(ethasks.size(), 0);
    ASSERT_EQ(ethbids.size(), 2);
    ASSERT_EQ(ethbids.at(0), Order("45691", Order::BID, "ETHUSD", 3, 180));
    ASSERT_EQ(ethbids.at(1), Order("11431", Order::BID, "ETHUSD", 4, 175));
}

TEST(MatchingEngineTest, OrderByArrival2) { 
    Solution s;

    auto o1 = Order("b1", Order::BID, "BTCUSD", 1, 10);
    auto o2 = Order("b2", Order::BID, "BTCUSD", 1, 11);

    s.execute(o1);
    s.execute(o2);

    ASSERT_EQ(s.trades.size(), 0);

    std::deque<Order> openByArrival = s.getOpenOrdersByArrival();
    ASSERT_EQ(openByArrival.size(), 2);
    ASSERT_EQ(openByArrival.at(0), Order("b1", Order::BID, "BTCUSD", 1, 10));
    ASSERT_EQ(openByArrival.at(1), Order("b2", Order::BID, "BTCUSD", 1, 11));

    OrderBook::OrdersT bids, asks;
    std::tie(bids, asks) = s.markets.getBidsAndAsks("BTCUSD");
    ASSERT_EQ(bids.size(), 2);
    ASSERT_EQ(bids.at(0), Order("b2", Order::BID, "BTCUSD", 1, 11));
    ASSERT_EQ(bids.at(1), Order("b1", Order::BID, "BTCUSD", 1, 10));
    ASSERT_EQ(asks.size(), 0);
}

// Not really a test... just convenient to put here for now.
TEST(MatchingEngineTest, PerfBigAskSmallBids) { 
    TradesT trades;
    Markets ms(
        [](const Order&) {},
        [](const Order&) {},
        [](const Order&) {},
        [&](const Trade& t) { trades.emplace_back(t); },
        [](const Error&) {}
    );

    Order::QtyT N = 1'000'000;
    auto bigAsk = Order("a1", Order::ASK, "BTCUSD", N, 10);
    auto smallBid = Order("b1", Order::BID, "BTCUSD", 1, 10);
    auto begin = std::chrono::high_resolution_clock::now();
    ms.execute(bigAsk);
    for (Order::QtyT i = 0; i < N; ++i) {
        ms.execute(smallBid);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur_ms = duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "millis: " << dur_ms << std::endl;
    std::cout << "tps:    " << (N/(dur_ms/1000.0)) << std::endl;
    ASSERT_EQ(trades.size(), N);

    // On my Mac M1 this reported:
    // millis: 74
    // tps:    1.35135e+06
    // Which I guess is not horrible.
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}