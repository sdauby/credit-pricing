#pragma once
#include "Pricers/Pricer.hpp"

struct Portfolio;
class PricingConfiguration;

namespace PricingEngine {
    PvResult price(const InstrumentMap& instruments, const PricingConfiguration& config);
}
