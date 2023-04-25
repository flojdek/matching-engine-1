#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <iostream>

#include "Markets.hpp"

/**
 * It's just a solution class to print trades and maintain extra data structures to print the open
 * orders in arrival order rather than normal balanced book order. 
 * !!! NOTE !!! These callbacks execute on the same thread as the order book matching
 * runs, but obviously ideally this would be done on a separate thread/process so it
 * doesn't block the matching - here just for simplicity, don't have that much time...
 */
struct Solution {
    Solution()
    : markets(
        [&](const Order& o) { onNew(o); },
        [&](const Order& o) { onPartialFill(o); },
        [&](const Order& o) { onFullFill(o); },
        [&](const Trade& t) { onTrade(t); },
        [&](const Error& e) { onError(e); }
    )
    {}

    void execute(const Order& incoming) {
        markets.execute(incoming);
    }

    void onNew(const Order& order) {
        // Got in as open order so we keep track of it for our state purposes.
        if (order.side == Order::ASK) {
            auto it = openAsksByArrival.emplace(openAsksByArrival.end(), order);
            openAskItByOrderId.emplace(order.orderId, it);
        } else if (order.side == Order::BID) {
            auto it = openBidsByArrival.emplace(openBidsByArrival.end(), order);
            openBidItByOrderId.emplace(order.orderId, it);
        }
    }

    void onTrade(const Trade& trade) {
        std::cout << trade.toString() << std::endl;
        trades.emplace_back(trade);
    }

    void onPartialFill(const Order& order) {
        // Just update quantities of our state so we keep proper track how much is left.
        if (order.side == Order::ASK) {
            auto it = openAskItByOrderId.find(order.orderId);
            if (it != openAskItByOrderId.end()) it->second->quantity = order.quantity;
        } else if (order.side == Order::BID) {
            auto it = openBidItByOrderId.find(order.orderId);
            if (it != openBidItByOrderId.end()) it->second->quantity = order.quantity;
        }
    }

    void onFullFill(const Order& order) {
        // Update our data structures.
        if (order.side == Order::ASK) {
            auto it = openAskItByOrderId.find(order.orderId);
            if (it != openAskItByOrderId.end()) {
                openAsksByArrival.erase(it->second);
                openAskItByOrderId.erase(it);
            }
        } else if (order.side == Order::BID) {
            auto it = openBidItByOrderId.find(order.orderId);
            if (it != openBidItByOrderId.end()) {
                openBidsByArrival.erase(it->second);
                openBidItByOrderId.erase(it);
            }
        }
    }

    void onError(const Error& error) {
        // Do nothing.
    }

    void printOpenOrdersByArrival() const {
        for (auto it = openAsksByArrival.begin(); it != openAsksByArrival.end(); it++) {
            std::cout << it->toString() << std::endl;
        }
        for (auto it = openBidsByArrival.begin(); it != openBidsByArrival.end(); it++) {
            std::cout << it->toString() << std::endl;
        }
    }

    OrderBook::BidsAndAsksT getBidsAndAsks(const std::string& instrument) const {
        return markets.getBidsAndAsks(instrument);
    }

    std::deque<Order> getOpenOrdersByArrival() const {
        std::deque<Order> ret;
        for (auto it = openAsksByArrival.begin(); it != openAsksByArrival.end(); it++) {
            ret.emplace_back(*it);
        }
        for (auto it = openBidsByArrival.begin(); it != openBidsByArrival.end(); it++) {
            ret.emplace_back(*it);
        }
        return ret;
    }

    Markets markets;
    TradesT trades;

    std::deque<Order> openAsksByArrival;
    std::map<Order::OrderIdT, std::deque<Order>::iterator> openBidItByOrderId;

    std::deque<Order> openBidsByArrival;
    std::map<Order::OrderIdT, std::deque<Order>::iterator> openAskItByOrderId;
};

#endif // SOLUTION_HPP