#pragma once

#include "Instruments/IRSwap.hpp"

class InterestRateCurve;

double annuity(const IRSwap& swap, const InterestRateCurve& curve);
double swapRate(const IRSwap& swap, const InterestRateCurve& curve);
