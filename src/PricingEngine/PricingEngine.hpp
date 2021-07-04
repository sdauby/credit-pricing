#pragma once
#include "Pricers/Pricer.hpp"
#include "Metrics/Metric.hpp"
#include "Metrics/Result.hpp"
#include "Elaboration/InstrumentElaborator.hpp"

class Portfolio;
class PricingConfiguration;

namespace PricingEngine {
    std::map<InstrumentId,Result> run(const std::vector<InstrumentId>& instruments,
                                      const InstrumentFactory& makeInstrument,
                                      const PricingConfiguration& config,
                                      const std::vector<Metric>& metrics);
}
