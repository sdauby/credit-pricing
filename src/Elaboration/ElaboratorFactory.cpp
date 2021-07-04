#include "ElaboratorFactory.hpp"

struct ElaboratorGeneralFactory::Impl {
    std::tuple<
        ElaboratorFactory<PricerId           >,
        ElaboratorFactory<PositionId         >,
        ElaboratorFactory<InstrumentId       >,
        ElaboratorFactory<HazardRateCurveId  >,
        ElaboratorFactory<InterestRateCurveId>,
        ElaboratorFactory<S3ModelId          >
    > factories;
};

ElaboratorGeneralFactory::~ElaboratorGeneralFactory() = default;
ElaboratorGeneralFactory::ElaboratorGeneralFactory() : pImpl(std::make_unique<Impl>()) {}

template<typename IdT> ElaboratorFactory<IdT>& ElaboratorGeneralFactory::getFactory() const
{
    return std::get<ElaboratorFactory<IdT>>(pImpl->factories);
}

template ElaboratorFactory<PricerId           >& ElaboratorGeneralFactory::getFactory<PricerId           >() const;
template ElaboratorFactory<PositionId         >& ElaboratorGeneralFactory::getFactory<PositionId         >() const;
template ElaboratorFactory<InstrumentId       >& ElaboratorGeneralFactory::getFactory<InstrumentId       >() const;
template ElaboratorFactory<HazardRateCurveId  >& ElaboratorGeneralFactory::getFactory<HazardRateCurveId  >() const;
template ElaboratorFactory<InterestRateCurveId>& ElaboratorGeneralFactory::getFactory<InterestRateCurveId>() const;
template ElaboratorFactory<S3ModelId          >& ElaboratorGeneralFactory::getFactory<S3ModelId          >() const;
