#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

struct Error {
    enum TypeT {
        INVALID_QTY,
        INVALID_PRICE,
        INVALID_SIDE,
        INVALID_TYPE
    };

    TypeT type;
};

#endif // ERROR_HPP