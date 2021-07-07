#pragma once

#include "Core/Currency.hpp"

#include <string>

class InterestRateCurve;

struct InterestRateCurveId {
    using ObjectType = InterestRateCurve;
    Currency ccy = {};
};

inline bool operator<(const InterestRateCurveId& x, 
                      const InterestRateCurveId& y)
{
    return x.ccy < y.ccy;
}

inline std::string to_string_(const InterestRateCurveId& id)
{
    std::string s = "InterestRateCurve{";
    s += + name(id.ccy);
    s += "}";
    return s;
}
