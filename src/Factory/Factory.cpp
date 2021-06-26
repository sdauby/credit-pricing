#include "Factory.hpp"
#include "Pricers/PricerKind.hpp"
#include "Pricers/PricingConfiguration.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"


struct Factory::Impl {
    explicit Impl(const PricingConfiguration& config) : config(config) {}
    const PricingConfiguration& config;
};

Factory::~Factory() = default;
Factory::Factory(const PricingConfiguration& config) : pImpl(std::make_unique<Impl>(config)) {}


template<> 
std::vector<VariantId> Factory::getPrecedents(const PricerId& pricerId, const Container& container) const
{
    std::vector<VariantId> precedents;
    std::vector<InstrumentId> instruments;
    for (const auto& instrumentId : container.ids<InstrumentId>()) {
        if ( pImpl->config.pricerKind( *container.get(instrumentId) ) == pricerId.kind ) {
            instruments.emplace_back(instrumentId);
            precedents.emplace_back(instrumentId);
        }
    }
    
    auto modelIds = [&] () {
        switch (pricerId.kind) {
            case PricerKind::IR: return IRPricer(container,instruments).requiredModels();
            case PricerKind::S3: return S3Pricer(container,instruments).requiredModels();
        }
    } ();
    std::move(modelIds.begin(),modelIds.end(),std::back_inserter(precedents));

    return precedents;
}

template<>
std::unique_ptr<Object<PricerId>> Factory::make(const PricerId& pricerId, const Container& container) const
{
    std::vector<InstrumentId> instruments;
    for (const auto& instrumentId : container.ids<InstrumentId>()) {
        if ( pImpl->config.pricerKind( *container.get(instrumentId) ) == pricerId.kind ) 
            instruments.emplace_back(instrumentId);
    }

    switch (pricerId.kind) {
        case PricerKind::IR: return std::make_unique<IRPricer>(container,instruments);
        case PricerKind::S3: return std::make_unique<S3Pricer>(container,instruments);
    }
}
