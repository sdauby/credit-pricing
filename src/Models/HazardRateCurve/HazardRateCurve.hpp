#pragma once
#include "Core/Date.hpp"

class HazardRateCurve final {
public:
    explicit HazardRateCurve(double lambda);

    double rate(Date t1, Date t2) const;
    double survivalProbability(Date t1, Date t2) const;

private:
    double lambda_ = 0.0;
};