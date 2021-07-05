#pragma once
#include "Metric.hpp"

class PVImpl : public MetricImpl {
public:
    std::map<InstrumentId,Result> compute(const Container& modelContainer) const override;
 };