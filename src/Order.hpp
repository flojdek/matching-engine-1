#ifndef ORDER_HPP
#define ORDER_HPP

#include <sstream>
#include <string>

class Order {
    public:
        typedef uint32_t PriceT;
        typedef uint64_t QtyT;
        typedef std::string OrderIdT;
        enum SideT { BID, ASK };
        enum OrderT { LIMIT };

        Order() = delete;
        Order(std::string oid, SideT sd, std::string in, QtyT qt, PriceT pr)
            : orderId(oid), side(sd), instrument(in), quantity(qt), price(pr)
        {}

        // NOTE: `= default` worked just fine on Mac but not in the container Debian.
        friend bool operator==(const Order& lhs, const Order& rhs) {
            return lhs.type == rhs.type && lhs.side == rhs.side && lhs.price == rhs.price && lhs.quantity == rhs.quantity
                && lhs.instrument == rhs.instrument && lhs.orderId == rhs.orderId;
        }

        std::string toString() const {
            std::stringstream ss;
            ss << orderId << " " << (side == BID ? "BUY" : "SELL") << " " << instrument << " " << quantity << " " << price;
            return ss.str();
        }

        OrderT type = LIMIT;
        SideT side; 
        PriceT price;
        QtyT quantity;
        std::string instrument;
        std::string orderId;

};

#endif // ORDER_HPP