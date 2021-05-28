#include "InterestRateCurve.hpp"
#include <cmath>

InterestRateCurve::InterestRateCurve(double rate) : rate_{rate} {}

double InterestRateCurve::discountFactor(Date t1, Date t2) const
{
    return std::exp( -rate_ * dayCountFactor(t1,t2) );
}

double forwardRate(const InterestRateCurve& curve, Date t1, Date t2)
{
    return 1.0/dayCountFactor(t1,t2) * ( 1.0 / curve.discountFactor(t1,t2) - 1.0 );
}