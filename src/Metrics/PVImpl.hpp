#pragma once
#include "Metric.hpp"

class PVImpl : public MetricImpl {
public:
    std::map<InstrumentId,Result> compute(const PricerId& pricerId,
                                          const Container& modelContainer) const override;
 };