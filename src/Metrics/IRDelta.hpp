#pragma once

#include "Metric.hpp"
#include "Elaboration/IdDag.hpp"

class BuilderGeneralFactory;

namespace Metrics {

class IRDelta : public Metric {
public:
    IRDelta(const IdDagAux&& requests, const BuilderGeneralFactory& factory);   

    std::map<InstrumentId,Result> compute(const Container& modelContainer) const override;

private:
    IdDagAux requests_;
    const BuilderGeneralFactory& factory_;
};

}
