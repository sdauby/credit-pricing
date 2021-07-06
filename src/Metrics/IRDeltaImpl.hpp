#pragma once

#include "Metric.hpp"
#include "Elaboration/IdDag.hpp"

class BuilderGeneralFactory;

class IRDeltaImpl : public MetricImpl {
public:
    IRDeltaImpl(const IdDagAux&& requests, const BuilderGeneralFactory& factory);   

    std::map<InstrumentId,Result> compute(const Container& modelContainer) const override;

private:
    IdDagAux requests_;
    const BuilderGeneralFactory& factory_;
};