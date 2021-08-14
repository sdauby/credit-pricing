#include "Elaboration/PricerBuilder.hpp"

#include "Pricers/Pricer.hpp"
#include "Pricers/PricerKind.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "Pricers/General/GeneralPricer.hpp"

PricerBuilder::PricerBuilder(const PricerId& id,
                             const PricingConfiguration& config) : 
                             id_(id),
                             config_(config)
{}

std::vector<VariantId> PricerBuilder::getRequestBatch(const Container& container)
{
    switch (state_) {
        case State::Initial:
            state_ = State::InstrumentsRequested;
            return std::vector<VariantId>(id_.instruments.cbegin(), id_.instruments.cend());
        case State::InstrumentsRequested:
            state_ = State::PricerRequestsRequested;
            switch (id_.kind) {
                case PricerKind::General: {
                    auto pricer = std::make_unique<GeneralPricer>(container,id_.instruments, config_);
                    auto pricerIds = pricer->requiredPricers();
                    pricer_ = std::move(pricer);
                    return std::vector<VariantId>(pricerIds.cbegin(),pricerIds.cend());
                }
                case PricerKind::IR: {
                    auto pricer = std::make_unique<IRPricer>(container,id_.instruments);
                    auto curveIds = pricer->requiredCurves();
                    pricer_ = std::move(pricer);
                    return std::vector<VariantId>(curveIds.cbegin(),curveIds.cend());
                }
                case PricerKind::S3: {
                    auto pricer = std::make_unique<S3Pricer>(container,id_.instruments); 
                    auto modelIds = pricer->requiredModels();
                    pricer_ = std::move(pricer);
                    return std::vector<VariantId>(modelIds.cbegin(),modelIds.cend());
                }
            }
        case State::PricerRequestsRequested:
            state_ = State::Final;
            return {};
        case State::Final:
            assert(!"unexpected state for getRequestBatch() call");
            return {};
    }
}

std::unique_ptr<Pricer> PricerBuilder::getObject(const Container&)
{
    assert(state_ == State::Final && "unexpected state for make() call");
    return std::move(pricer_);
}

