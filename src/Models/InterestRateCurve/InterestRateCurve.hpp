#pragma once
#include "Core/Date.hpp"

class InterestRateCurve final {
public:
    explicit InterestRateCurve(double rate);
    
    double discountFactor(Date t1, Date t2) const;

private:
    double rate_ = 0.0;
};

double forwardRate(const InterestRateCurve& curve, Date t1, Date t2);