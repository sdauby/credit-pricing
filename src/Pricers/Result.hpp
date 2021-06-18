#pragma once
#include <variant>
#include <map>
#include "Core/Currency.hpp"

struct PVKey {
    Currency ccy;
};

inline bool operator< (const PVKey& x, const PVKey& y)
{
    return x.ccy < y.ccy;
}

struct IRDeltaKey {
    Currency ccy;
    std::string bucket; // improve: string too unstructured
};

inline bool operator< (const IRDeltaKey& x, const IRDeltaKey& y)
{
    const auto tuple = [](const auto& k) { return std::tuple{k.ccy,k.bucket}; };
    return tuple(x)<tuple(y);
}


using ResultKey = std::variant<
    PVKey,
    IRDeltaKey
>;

using Result = std::map<ResultKey,double>;