#include "PricerSubFactory.hpp"
#include "Pricers/PricingConfiguration.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"

PricerSubFactory::PricerSubFactory(const PricingConfiguration& config) :
    config_(config)
{}

std::vector<VariantId> PricerSubFactory::getPrecedents(const PricerId& pricerId, const Container& container) const
{
    std::vector<VariantId> precedents;
    std::vector<InstrumentId> instruments;
    for (const auto& instrumentId : container.ids<InstrumentId>()) {
        if ( config_.pricerKind( *container.get(instrumentId) ) == pricerId.kind ) {
            instruments.emplace_back(instrumentId);
            precedents.emplace_back(instrumentId);
        }
    }
    
    auto modelIds = [&] () {
        switch (pricerId.kind) {
            case PricerKind::IR: return IRPricer(container,instruments).precedents();
            case PricerKind::S3: return S3Pricer(container,instruments).precedents();
            case PricerKind::General: assert(!"case not handled"); return std::vector<VariantId>{};
        }
    } ();
    std::move(modelIds.begin(),modelIds.end(),std::back_inserter(precedents));

    return precedents;
}

std::unique_ptr<Pricer> PricerSubFactory::make(const PricerId& pricerId, const Container& container) const
{
    std::vector<InstrumentId> instruments;
    for (const auto& instrumentId : container.ids<InstrumentId>()) {
        if ( config_.pricerKind( *container.get(instrumentId) ) == pricerId.kind ) 
            instruments.emplace_back(instrumentId);
    }

    switch (pricerId.kind) {
        case PricerKind::IR: return std::make_unique<IRPricer>(container,instruments);
        case PricerKind::S3: return std::make_unique<S3Pricer>(container,instruments);
        case PricerKind::General: assert(!"case not handled"); return std::unique_ptr<Pricer>{};
    }
}
