#ifndef MATCHINGENGINE_HPP
#define MATCHINGENGINE_HPP

#include <unordered_map>

#include "OrderBook.hpp"

/**
 * Maintains books per each instrument mapping. 
 * Can pass callbacks to react to various events.
 */
class Markets {
    public:
        Markets() = delete;
        Markets(
            OrderBook::OnOrderChangeFuncT onNew,
            OrderBook::OnOrderChangeFuncT onPartialFill,
            OrderBook::OnOrderChangeFuncT onFullFill,
            OrderBook::OnTradeFuncT onTrade,
            OrderBook::OnErrorFuncT onError
        ) : m_onNew(onNew)
          , m_onTrade(onTrade)
          , m_onPartialFill(onPartialFill)
          , m_onFullFill(onFullFill)
          , m_onError(onError)
        {}

        void execute(const Order& incoming) {
            auto it = m_orderBookByInstrument.find(incoming.instrument);
            if (it == m_orderBookByInstrument.end()) {
                std::unique_ptr<OrderBook> ob(
                    new OrderBook(
                        [&](const Order& order){ m_onNew(order); },
                        [&](const Order& order){ m_onPartialFill(order); },
                        [&](const Order& order){ m_onFullFill(order); },
                        [&](const Trade& trade){ m_onTrade(trade); },
                        [&](const Error& error){ m_onError(error); }
                    )
                );
                ob->execute(incoming);
                m_orderBookByInstrument.emplace(incoming.instrument, std::move(ob));
            } else {
                it->second->execute(incoming);
            }
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<OrderBook>> m_orderBookByInstrument;

        OrderBook::OnOrderChangeFuncT m_onNew;
        OrderBook::OnOrderChangeFuncT m_onFullFill;
        OrderBook::OnOrderChangeFuncT m_onPartialFill;
        OrderBook::OnTradeFuncT m_onTrade;
        OrderBook::OnErrorFuncT m_onError;
};

#endif // MATCHINGENGINE_HPP