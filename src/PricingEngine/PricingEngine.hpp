#pragma once
#include "Instruments/Portfolio.hpp"
#include "Pricers/Pricer.hpp"
#include "Pricers/Metric.hpp"
#include "Pricers/Result.hpp"

class PricingConfiguration;

namespace PricingEngine {

    std::map<InstrumentId, Result> price(const InstrumentMap& instruments, 
                                         const PricingConfiguration& config,
                                         const std::vector<Metric>& metrics);
}
