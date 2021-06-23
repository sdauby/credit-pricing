#pragma once
#include "Instruments/Portfolio.hpp"
#include "Pricers/Pricer.hpp"
#include "Metrics/Metric.hpp"
#include "Metrics/Result.hpp"

class PricingConfiguration;

namespace PricingEngine {

    std::map<InstrumentId, Result> run(const InstrumentMap& instruments, 
                                         const PricingConfiguration& config,
                                         const std::vector<Metric>& metrics);
}
