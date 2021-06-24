#include "Container.hpp"
#include "InterestRateCurveId.hpp"
#include "HazardRateCurveId.hpp"
#include "S3ModelId.hpp"
#include "InstrumentId.hpp"
#include "PricerId.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"
#include "Instruments/Instrument.hpp"
#include "Pricers/Pricer.hpp"
#include <set>

struct Container::Impl {
    std::tuple<
        ObjectMap<InterestRateCurveId>,
        ObjectMap<HazardRateCurveId  >,
        ObjectMap<S3ModelId          >,
        ObjectMap<InstrumentId       >,
        ObjectMap<PricerId           >
    > objects;
    const Container* baseContainer = nullptr;
};

Container::Container() : pImpl(std::make_unique<Impl>()) {}

Container::Container(const Container& baseContainer) : Container()
{
    pImpl->baseContainer = &baseContainer;
}

Container::~Container() = default;
Container::Container(Container&& container) = default;
Container& Container::operator=(Container&& container) = default;


template<typename ObjectIdT>
const ObjectType<ObjectIdT>* Container::get(const ObjectIdT& id) const
{
    const auto& objectMap = std::get<ObjectMap<ObjectIdT>>(pImpl->objects);
    const auto& i = objectMap.find(id);
    if (i==objectMap.end()) {
        if (pImpl->baseContainer)
            return pImpl->baseContainer->get(id);
        else
            return nullptr;
    } else {
        return i->second.get();
    }
}

template<typename ObjectIdT>
void Container::set(const ObjectIdT& id, ObjectPtr<ObjectIdT>&& object)
{
    auto& objectMap = std::get<ObjectMap<ObjectIdT>>(pImpl->objects);
    auto node = std::pair{id,std::move(object)};
    objectMap.emplace(std::move(node));
}

template<typename ObjectIdT>
std::vector<ObjectIdT> Container::ids() const
{
    std::set<ObjectIdT> ids;
    const auto& objectMap = std::get<ObjectMap<ObjectIdT>>(pImpl->objects);
    for (const auto& [id, object] : objectMap)
        ids.insert(id);
    if (pImpl->baseContainer) {
        const auto baseContainerIds = pImpl->baseContainer->ids<ObjectIdT>();
        ids.insert(baseContainerIds.cbegin(),baseContainerIds.cend());
    }
    return std::vector<ObjectIdT>(ids.begin(),ids.end());
}

template const ObjectType<InterestRateCurveId>* Container::get(const InterestRateCurveId& id) const;
template const ObjectType<HazardRateCurveId  >* Container::get(const HazardRateCurveId  & id) const;
template const ObjectType<S3ModelId          >* Container::get(const S3ModelId          & id) const;
template const ObjectType<InstrumentId       >* Container::get(const InstrumentId       & id) const;
template const ObjectType<PricerId           >* Container::get(const PricerId           & id) const;

template void Container::set(const InterestRateCurveId& id, ObjectPtr<InterestRateCurveId>&& object);
template void Container::set(const HazardRateCurveId  & id, ObjectPtr<HazardRateCurveId  >&& object);
template void Container::set(const S3ModelId          & id, ObjectPtr<S3ModelId          >&& object);
template void Container::set(const InstrumentId       & id, ObjectPtr<InstrumentId       >&& object);
template void Container::set(const PricerId           & id, ObjectPtr<PricerId           >&& object);

template std::vector<InterestRateCurveId> Container::ids() const;
template std::vector<HazardRateCurveId  > Container::ids() const;
template std::vector<S3ModelId          > Container::ids() const;
template std::vector<InstrumentId       > Container::ids() const;
template std::vector<PricerId           > Container::ids() const;
