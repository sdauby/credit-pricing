#pragma once

#include "Container/IdTypes/InstrumentId.hpp"
#include "Metrics/Result.hpp"

class Container;

namespace Metrics {

enum class MetricKind {
    PV,
    IRDelta,
};

class Metric {
public:
    virtual ~Metric() = default;
    Metric() = default;
    Metric(const Metric&) = delete;
    Metric& operator=(const Metric&) = delete;
    
    virtual std::map<InstrumentId,Result> compute(const Container& container) const = 0;
};

}