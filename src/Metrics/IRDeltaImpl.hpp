#pragma once
#include "Metric.hpp"

class IRDeltaImpl : public MetricImpl {
public:
    IRDeltaImpl() = default;

    std::map<InstrumentId,Result> compute(const Pricer& pricer,
                                          const Container& modelContainer) const override;
 
};