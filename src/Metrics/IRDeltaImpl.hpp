#pragma once

#include "Metric.hpp"

class ContainerDag;
class Factory;

class IRDeltaImpl : public MetricImpl {
public:
    IRDeltaImpl(const ContainerDag& updateDag, const Factory& factory);   

    std::map<InstrumentId,Result> compute(const Pricer& pricer,
                                          const Container& modelContainer) const override;

private:
    const ContainerDag& updateDag_;
    const Factory& factory_;
};