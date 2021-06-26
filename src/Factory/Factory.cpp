#include "Factory.hpp"
#include "Pricers/PricerKind.hpp"
#include "Pricers/PricingConfiguration.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "ModelFactory/ModelFactory.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"

Factory::Factory(const PricingConfiguration& config) : config_(config) {}

template<>
std::vector<VariantId> Factory::getPrecedents(const InstrumentId&, const Container&) const
{ 
    return {}; 
}

template<> 
std::vector<VariantId> Factory::getPrecedents(const PricerId& pricerId, const Container& container) const
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
            case PricerKind::IR: return IRPricer(container,instruments).requiredModels();
            case PricerKind::S3: return S3Pricer(container,instruments).requiredModels();
        }
    } ();
    std::move(modelIds.begin(),modelIds.end(),std::back_inserter(precedents));

    return precedents;
}

template<typename ModelIdT>
std::vector<VariantId> Factory::getPrecedents(const ModelIdT& id, const Container&) const
{
    std::vector<ModelId> modelIds = ModelFactory().precedents(id);
    std::vector<VariantId> precedents;
    std::transform(modelIds.begin(),modelIds.end(),std::back_inserter(precedents),
        [](const ModelId& modelId) {
            return std::visit([](const auto& id) {
                return VariantId(id);
            },
            modelId);
        });
    return precedents;
}

template std::vector<VariantId> Factory::getPrecedents(const HazardRateCurveId&, const Container&) const;
template std::vector<VariantId> Factory::getPrecedents(const InterestRateCurveId&, const Container&) const;
template std::vector<VariantId> Factory::getPrecedents(const S3ModelId&, const Container&) const;



template<>
std::unique_ptr<Object<InstrumentId>> Factory::make(const InstrumentId&, const Container&) const
{
    return {};
}

template<>
std::unique_ptr<Object<PricerId>> Factory::make(const PricerId& pricerId, const Container& container) const
{
    std::vector<InstrumentId> instruments;
    for (const auto& instrumentId : container.ids<InstrumentId>()) {
        if ( config_.pricerKind( *container.get(instrumentId) ) == pricerId.kind ) 
            instruments.emplace_back(instrumentId);
    }

    switch (pricerId.kind) {
        case PricerKind::IR: return std::make_unique<IRPricer>(container,instruments);
        case PricerKind::S3: return std::make_unique<S3Pricer>(container,instruments);
    }
}

template<typename ModelIdT>
std::unique_ptr<Object<ModelIdT>> Factory::make(const ModelIdT& id, const Container& container) const
{
    std::unique_ptr<Model> model = ModelFactory().make(id,container);
    Model* model_ = model.release();
    Object<ModelIdT>* object = dynamic_cast<Object<ModelIdT>*>(model_);
    std::unique_ptr<Object<ModelIdT>> object_(object);
    return object_;
}

template std::unique_ptr<Object<HazardRateCurveId>> Factory::make(const HazardRateCurveId& id, const Container& container) const;
template std::unique_ptr<Object<InterestRateCurveId>> Factory::make(const InterestRateCurveId& id, const Container& container) const;
template std::unique_ptr<Object<S3ModelId>> Factory::make(const S3ModelId& id, const Container& container) const;
