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
            return std::vector<VariantId>(id_.instruments.begin(), id_.instruments.end());
        case State::InstrumentsRequested:
            switch (id_.kind) {
                case PricerKind::General: 
                    pricer_ = std::make_unique<GeneralPricer>(container,id_.instruments, config_);
                    break;
                case PricerKind::IR: 
                    pricer_ = std::make_unique<IRPricer>(container,id_.instruments); 
                    break;
                case PricerKind::S3: 
                    pricer_ = std::make_unique<S3Pricer>(container,id_.instruments); 
                    break;
            }
            state_ = State::PricerPrecedentsRequested;
            return pricer_->precedents();
        case State::PricerPrecedentsRequested:
            state_ = State::AllRequestsDone;
            return {};
        case State::AllRequestsDone:
            assert(!"unexpected state for getRequestBatch() call");
            return {};
    }
}

std::unique_ptr<Pricer> PricerBuilder::getObject(const Container&)
{
    assert(state_ == State::AllRequestsDone && "unexpected state for make() call");
    return std::move(pricer_);
}

