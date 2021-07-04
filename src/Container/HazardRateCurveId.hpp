#pragma once
#include "Core/Currency.hpp"
#include "Core/Issuer.hpp"
#include <tuple>

class HazardRateCurve;

struct HazardRateCurveId {
    using ObjectType = HazardRateCurve;
    Issuer issuer;
    Currency ccy = {};
};

inline bool operator<(const HazardRateCurveId& x, 
                      const HazardRateCurveId& y)
{
    return std::tuple{x.issuer, x.ccy} 
        <  std::tuple{y.issuer, y.ccy};
}

inline std::string to_string_(const HazardRateCurveId& id)
{
    std::string s;
    s += "HazardRateCurve{";
    s += id.issuer;
    s += + ",";
    s += name(id.ccy);
    s += "}";
    return s;
}
