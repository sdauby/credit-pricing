#pragma once
#include "Instruments/IRSwap.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"

double annuity(const IRSwap& swap, const InterestRateCurve& curve);
double swapRate(const IRSwap& swap, const InterestRateCurve& curve);
