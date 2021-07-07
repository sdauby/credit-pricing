#pragma once

#include "Pricers/PricerKind.hpp"
#include "Container/InstrumentId.hpp"

#include <tuple>
#include <vector>

class Pricer;

struct PricerId {
    using ObjectType = Pricer;
    PricerKind kind = {};
    std::vector<InstrumentId> instruments;
};

inline bool operator<(const PricerId& x, 
                      const PricerId& y)
{
    return std::tuple{x.kind,x.instruments} < std::tuple{y.kind,y.instruments};
}

inline std::string to_string_(const PricerId& id)
{
    std::string s = "Pricer{";
    s += name(id.kind);
    s += ",[";
    for (const auto& instrument : id.instruments)
        s += to_string_(instrument) + ",";
    s += "]}";
    return s;
}
