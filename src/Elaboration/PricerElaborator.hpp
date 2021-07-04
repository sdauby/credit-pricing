#pragma once

#include "ElaboratorFactory.hpp"

class PricingConfiguration;

class PricerElaborator : public Elaborator<PricerId> {
public:
    PricerElaborator(const PricerId& id, const PricingConfiguration& config);
private:
    std::vector<VariantId> getRequestBatch(const Container& container) override;
    std::unique_ptr<Pricer> make(const Container& container) override; 
    
    PricerId id_;
    const PricingConfiguration& config_;
    std::unique_ptr<Pricer> pricer_;
    enum class State { 
        Initial, 
        InstrumentsRequested, 
        PricerPrecedentsRequested, 
        AllRequestsDone,
    };
    State state_ = State::Initial;
};
