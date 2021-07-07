#pragma once

#include "Core/Date.hpp"

#include <vector>

class InterestRateCurve {
public:
    explicit InterestRateCurve(double rate);
    InterestRateCurve(std::vector<Date>&& T, std::vector<double>&& r);
    
    double discountFactor(Date t, Date u) const;

    std::unique_ptr<InterestRateCurve> applyRateShift(double rateShift) const;
    friend class IRCurveCalibration;

private:
    std::vector<Date> T_;
    std::vector<double> r_;
};

double forwardRate(const InterestRateCurve& curve, Date t, Date u);
