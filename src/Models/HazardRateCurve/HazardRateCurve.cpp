#include "HazardRateCurve.hpp"

HazardRateCurve::HazardRateCurve(double rate) : rate_{rate} {}

double HazardRateCurve::rate(Date t1, Date t2) const
{
    return rate_;
}