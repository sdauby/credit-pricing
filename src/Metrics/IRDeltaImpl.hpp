#pragma once

#include "Metric.hpp"
#include "Elaboration/IdDag.hpp"

class ElaboratorGeneralFactory;

class IRDeltaImpl : public MetricImpl {
public:
    IRDeltaImpl(const IdDagAux&& requests, const ElaboratorGeneralFactory& factory);   

    std::map<InstrumentId,Result> compute(const PricerId& pricerId,
                                          const Container& modelContainer) const override;

private:
    IdDagAux requests_;
    const ElaboratorGeneralFactory& factory_;
};