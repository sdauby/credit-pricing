#pragma once

#include "Container/Ids/InstrumentId.hpp"
#include "Elaboration/InstrumentBuilder.hpp"
#include "Metrics/Result.hpp"

namespace Metrics { enum class MetricKind; }
class PricingConfiguration;

namespace PricingEngine {

std::map<InstrumentId,Metrics::Result> run(const std::vector<InstrumentId>& instruments,
                                           const InstrumentFactory& makeInstrument,
                                           const PricingConfiguration& config,
                                           const std::vector<Metrics::MetricKind>& metrics);

}
