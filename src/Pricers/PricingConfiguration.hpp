#pragma once
#include "Pricers/PricerKind.hpp"

class Instrument;

class PricingConfiguration final {
public:
    PricingConfiguration(PricerKind preferredKind);
    PricerKind pricerKind(const Instrument& instrument) const;
private:
    PricerKind preferredKind_;
};
