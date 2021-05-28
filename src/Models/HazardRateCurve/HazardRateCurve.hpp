#pragma once
#include "Core/Date.hpp"

class HazardRateCurve final {
public:
    explicit HazardRateCurve(double rate);

    double rate(Date t1, Date t2) const;

private:
    double rate_ = 0.0;
};