#pragma once
#include "Core/Currency.hpp"

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
