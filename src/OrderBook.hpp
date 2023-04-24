#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <functional>

#include "Error.hpp"
#include "Order.hpp"
#include "Trade.hpp"

class OrderBook {
    public:
        typedef std::function<void(const Order&)> OnOrderChangeFuncT;
        typedef std::function<void(const Trade&)> OnTradeFuncT;
        typedef std::function<void(const Error&)> OnErrorFuncT;
        typedef std::deque<Order> OrdersT;
        typedef std::pair<OrdersT, OrdersT> BidsAndAsksT;

        OrderBook() = delete;
        /**
         * The callbacks are synchronous in this case but ideally we would be publishing events/messages
         * on some bus and other downstream can update their state accordingly. 
         */
        OrderBook(
            OnOrderChangeFuncT onNew,
            OnOrderChangeFuncT onPartialFill,
            OnOrderChangeFuncT onFullFill,
            OnTradeFuncT onTrade,
            OnErrorFuncT onError
        ) : m_onNew(onNew)
          , m_onTrade(onTrade)
          , m_onPartialFill(onPartialFill)
          , m_onFullFill(onFullFill)
          , m_onError(onError)
        {}

        void execute(Order incoming) {
            switch (incoming.side) {
                case Order::ASK:
                    return execute(incoming, m_bids, m_asks);
                case Order::BID:
                    return execute(incoming, m_asks, m_bids);
                default:
                    m_onError(Error { .type = Error::INVALID_SIDE });
                    break;
            }
        }

        BidsAndAsksT getBidsAndAsks() const {
            OrdersT bids, asks;
            for (auto it = m_bids.begin(); it != m_bids.end(); ++it) bids.emplace_back(it->second);
            for (auto it = m_asks.begin(); it != m_asks.end(); ++it) asks.emplace_back(it->second);
            return std::make_pair(bids, asks);
        }

    private:
        template <typename T>
        using BookSideT = std::multimap<Order::PriceT, Order, T>;

        OnOrderChangeFuncT m_onNew;
        OnOrderChangeFuncT m_onFullFill;
        OnOrderChangeFuncT m_onPartialFill;
        OnTradeFuncT m_onTrade;
        OnErrorFuncT m_onError;

        BookSideT<std::less<Order::PriceT>> m_asks;
        BookSideT<std::greater<Order::PriceT>> m_bids;

        bool priceOkForMatch(const Order& incoming, Order::PriceT standingPrice) const {
            return incoming.side == 0 ?  incoming.price >= standingPrice : incoming.price <= standingPrice;
        }

        template <typename A, typename B>
        void execute(
            Order& incoming,
            BookSideT<A>& standingBook,
            BookSideT<B>& incomingBook
        ) {
            auto it = standingBook.begin();
            while (it != standingBook.end()) {
                if (incoming.quantity == 0) break;
                Order& standing = it->second;
                if (priceOkForMatch(incoming, standing.price)) {
                    Order::QtyT qtyTraded;
                    if (incoming.quantity > standing.quantity) { // incoming PF, standing FF
                        qtyTraded = standing.quantity;
                        incoming.quantity -= qtyTraded;
                        standing.quantity -= qtyTraded;
                        m_onPartialFill(incoming);
                        m_onFullFill(standing);
                        it = standingBook.erase(it);
                    } else if (incoming.quantity == standing.quantity) { // incoming FF, standing FF
                        qtyTraded = standing.quantity;
                        incoming.quantity -= qtyTraded;
                        standing.quantity -= qtyTraded;
                        m_onFullFill(incoming);
                        m_onFullFill(standing);
                        it = standingBook.erase(it);
                    } else { // incoming FF, standing PF
                        qtyTraded = incoming.quantity;
                        incoming.quantity -= qtyTraded;
                        standing.quantity -= qtyTraded;
                        m_onFullFill(incoming);
                        m_onPartialFill(standing);
                        it++;
                    }
                    m_onTrade(Trade(incoming.instrument, incoming.orderId, standing.orderId, qtyTraded, standing.price));
                } else {
                    break;
                }
            }
            if (incoming.quantity > 0) {
                m_onNew(incoming);
                incomingBook.emplace(incoming.price, incoming);
            }
        }
};

#endif // ORDERBOOK_HPP
