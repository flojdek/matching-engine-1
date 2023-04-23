#include <iostream>
#include <stdio.h>
#include <deque>
#include <unordered_map>
#include <chrono>
#include <map>
#include <optional>
#include <string>
#include <sstream>
#include <memory>

#include "Solution.hpp"

using namespace std::chrono;

int main() {
  std::cerr << "====== Match Engine =====" << std::endl;
  std::cerr << "Enter 'exit' to quit" << std::endl;
  Solution s;
  std::string line;
  while (getline(std::cin, line) && line != "exit") {
    std::stringstream ss(line);

    std::string orderId;
    ss >> orderId;

    std::string sd;
    Order::SideT side; 
    ss >> sd;
    if (sd == "BUY") side = Order::BID;
    if (sd == "SELL") side = Order::ASK;

    std::string instrument;
    ss >> instrument;

    Order::QtyT quantity;
    ss >> quantity;

    Order::PriceT price;
    ss >> price;

    s.execute(Order(orderId, side, instrument, quantity, price));
  }
  std::cout << std::endl;
  s.printOpenOrdersByArrival();
  return 0;
}