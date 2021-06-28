#pragma once

#include <set>
#include "Result.hpp"
#include "Container/InstrumentId.hpp"

class Pricer;
class Container;

enum class Metric {
    PV,
    IRDelta,
};

class MetricImpl {
public:
    virtual ~MetricImpl() = default;
    MetricImpl() = default;
    MetricImpl(const MetricImpl&) = delete;
    MetricImpl& operator=(const MetricImpl&) = delete;
    
    virtual std::map<InstrumentId,Result> compute(const Pricer& pricer,
                                                  const Container& container) const = 0;
};
