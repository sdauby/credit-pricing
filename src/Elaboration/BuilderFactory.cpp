#include "Elaboration/BuilderFactory.hpp"

struct BuilderGeneralFactory::Impl {
    std::tuple<
        BuilderFactory<PricerId           >,
        BuilderFactory<InstrumentId       >,
        BuilderFactory<HazardRateCurveId  >,
        BuilderFactory<InterestRateCurveId>,
        BuilderFactory<S3ModelId          >
    > factories;
};

BuilderGeneralFactory::~BuilderGeneralFactory() = default;
BuilderGeneralFactory::BuilderGeneralFactory() : pImpl(std::make_unique<Impl>()) {}

template<typename IdT> BuilderFactory<IdT>& BuilderGeneralFactory::getFactory() const
{
    return std::get<BuilderFactory<IdT>>(pImpl->factories);
}

template BuilderFactory<PricerId           >& BuilderGeneralFactory::getFactory<PricerId           >() const;
template BuilderFactory<InstrumentId       >& BuilderGeneralFactory::getFactory<InstrumentId       >() const;
template BuilderFactory<HazardRateCurveId  >& BuilderGeneralFactory::getFactory<HazardRateCurveId  >() const;
template BuilderFactory<InterestRateCurveId>& BuilderGeneralFactory::getFactory<InterestRateCurveId>() const;
template BuilderFactory<S3ModelId          >& BuilderGeneralFactory::getFactory<S3ModelId          >() const;
