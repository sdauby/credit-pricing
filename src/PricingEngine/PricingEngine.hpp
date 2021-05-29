#pragma once
#include "Pricers/Pricer.hpp"

struct Portfolio;
class PricingConfiguration;

namespace PricingEngine {
    PvResult price(const Portfolio& p, const PricingConfiguration& config);
}
