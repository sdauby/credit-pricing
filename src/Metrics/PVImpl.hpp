#pragma once
#include "Metric.hpp"

class PVImpl : public MetricImpl {
public:
    PVImpl() = default;

    std::map<InstrumentId,Result> compute(const Pricer& pricer,
                                          const Container& modelContainer) const override;
 };