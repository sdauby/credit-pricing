#pragma once

#include "Metric.hpp"

namespace Metrics {

class PV : public Metric {
public:
    std::map<InstrumentId,Result> compute(const Container& container) const override;
};

}
