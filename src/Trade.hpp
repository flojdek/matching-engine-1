#ifndef TRADE_HPP
#define TRADE_HPP

#include "Order.hpp"

class Trade {
    public:
        Trade() = delete;
        Trade(std::string in, std::string oid, std::string coid, Order::QtyT qt, Order::PriceT pr)
            : instrument(in), orderId(oid), contraOrderId(coid), qty(qt), price(pr) {}

        // NOTE: `= default` worked just fine on Mac but not in the container Debian.
        friend bool operator==(const Trade& lhs, const Trade& rhs) {
            return lhs.instrument == rhs.instrument && lhs.orderId == rhs.orderId && lhs.contraOrderId == rhs.contraOrderId
                && lhs.qty == rhs.qty && lhs.price == rhs.price;
        }

        std::string toString() const {
            std::stringstream ss;
            ss << "TRADE " << instrument << " " << orderId << " " << contraOrderId << " " << qty << " " << price;
            return ss.str();
        }

        std::string instrument;
        std::string orderId;
        std::string contraOrderId;
        Order::QtyT qty;
        Order::PriceT price;
};

typedef std::deque<Trade> TradesT;

#endif // TRADE_HPP