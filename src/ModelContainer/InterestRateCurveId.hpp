#pragma once
#include "Core/Currency.hpp"

class InterestRateCurve;

struct InterestRateCurveId {
    using ModelType = InterestRateCurve;
    Currency ccy = {};
};

inline bool operator<(const InterestRateCurveId& x, 
                      const InterestRateCurveId& y)
{
    return x.ccy < y.ccy;
}
