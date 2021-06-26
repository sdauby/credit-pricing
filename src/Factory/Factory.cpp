#include "Factory.hpp"
#include "Pricers/Pricer.hpp"
#include "Instruments/Instrument.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/S3/S3Model.hpp"

namespace {
    template <typename IdT>
    using SubFactoryPtr = std::unique_ptr<SubFactory<IdT>>;
}

struct Factory::Impl {
    std::tuple<
        SubFactoryPtr<PricerId           >,
        SubFactoryPtr<InstrumentId       >,
        SubFactoryPtr<HazardRateCurveId  >,
        SubFactoryPtr<InterestRateCurveId>,
        SubFactoryPtr<S3ModelId          >
    > subFactories;
};

Factory::~Factory() = default;
Factory::Factory() : pImpl(std::make_unique<Impl>()) {}

template<typename IdT>
void Factory::setSubFactory(std::unique_ptr<SubFactory<IdT>> subFactory)
{
    std::get<SubFactoryPtr<IdT>>(pImpl->subFactories) = std::move(subFactory);
}

template<typename IdT>
const SubFactory<IdT>& Factory::subFactory() const
{
    return *std::get<SubFactoryPtr<IdT>>(pImpl->subFactories);
}

template void Factory::setSubFactory(std::unique_ptr<SubFactory<PricerId           >> subFactory);
template void Factory::setSubFactory(std::unique_ptr<SubFactory<InstrumentId       >> subFactory);
template void Factory::setSubFactory(std::unique_ptr<SubFactory<HazardRateCurveId  >> subFactory);
template void Factory::setSubFactory(std::unique_ptr<SubFactory<InterestRateCurveId>> subFactory);
template void Factory::setSubFactory(std::unique_ptr<SubFactory<S3ModelId          >> subFactory);

template const SubFactory<PricerId           >& Factory::subFactory() const;
template const SubFactory<InstrumentId       >& Factory::subFactory() const;
template const SubFactory<HazardRateCurveId  >& Factory::subFactory() const;
template const SubFactory<InterestRateCurveId>& Factory::subFactory() const;
template const SubFactory<S3ModelId          >& Factory::subFactory() const;

