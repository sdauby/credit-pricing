#include "HazardRateCurve.hpp"
#include <cmath>

HazardRateCurve::HazardRateCurve(double lambda) : lambda_{lambda} {}

double HazardRateCurve::survivalProbability(Date t1, Date t2) const
{
    return std::exp( -lambda_ * dayCountFactor(t1,t2) );
}

double HazardRateCurve::rate(Date t1, Date t2) const
{
    return 1.0/dayCountFactor(t1,t2) * ( 1.0 / survivalProbability(t1,t2) - 1.0 );
}