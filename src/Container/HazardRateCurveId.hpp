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
