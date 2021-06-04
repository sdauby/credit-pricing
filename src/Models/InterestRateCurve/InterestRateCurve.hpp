#pragma once
#include "Core/Date.hpp"
#include <vector>

class InterestRateCurve final {
public:
    explicit InterestRateCurve(double rate);
    InterestRateCurve(std::vector<Date>&& T, std::vector<double>&& r);
    
    double discountFactor(Date t, Date u) const;

private:
    std::vector<Date> T_;
    std::vector<double> r_;
};

double forwardRate(const InterestRateCurve& curve, Date t, Date u);