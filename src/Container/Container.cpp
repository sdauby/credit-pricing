#include "Container/Container.hpp"

#include "Container/IdTypes.hpp"
#include "Container/ObjectTypes.hpp"

#include <map>
#include <set>

namespace {
    template<typename IdT>
    using ObjectMap = std::map<IdT,std::unique_ptr<Object<IdT>>>;
}

struct Container::Impl {
    std::tuple<
        ObjectMap<PricerId           >,
        ObjectMap<InstrumentId       >,
        ObjectMap<InterestRateCurveId>,
        ObjectMap<HazardRateCurveId  >,
        ObjectMap<S3ModelId          >
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

template<typename IdT>
Object<IdT>* Container::get(const IdT& id) const
{
    const auto& objectMap = std::get<ObjectMap<IdT>>(pImpl->objects);
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

template<typename IdT>
void Container::set(const IdT& id, std::unique_ptr<Object<IdT>>&& object)
{
    auto& objectMap = std::get<ObjectMap<IdT>>(pImpl->objects);
    auto node = std::pair{id,std::move(object)};
    objectMap.emplace(std::move(node));
}

template<typename IdT>
std::vector<IdT> Container::ids() const
{
    std::set<IdT> ids;
    const auto& objectMap = std::get<ObjectMap<IdT>>(pImpl->objects);
    for (const auto& [id, object] : objectMap)
        ids.insert(id);
    if (pImpl->baseContainer) {
        const auto baseContainerIds = pImpl->baseContainer->ids<IdT>();
        ids.insert(baseContainerIds.cbegin(),baseContainerIds.cend());
    }
    return std::vector<IdT>(ids.begin(),ids.end());
}

template Object<PricerId           >* Container::get(const PricerId           & id) const;
template Object<InstrumentId       >* Container::get(const InstrumentId       & id) const;
template Object<InterestRateCurveId>* Container::get(const InterestRateCurveId& id) const;
template Object<HazardRateCurveId  >* Container::get(const HazardRateCurveId  & id) const;
template Object<S3ModelId          >* Container::get(const S3ModelId          & id) const;

template void Container::set(const PricerId           & id, std::unique_ptr<Object<PricerId           >>&& object);
template void Container::set(const InstrumentId       & id, std::unique_ptr<Object<InstrumentId       >>&& object);
template void Container::set(const InterestRateCurveId& id, std::unique_ptr<Object<InterestRateCurveId>>&& object);
template void Container::set(const HazardRateCurveId  & id, std::unique_ptr<Object<HazardRateCurveId  >>&& object);
template void Container::set(const S3ModelId          & id, std::unique_ptr<Object<S3ModelId          >>&& object);

template std::vector<PricerId           > Container::ids() const;
template std::vector<InstrumentId       > Container::ids() const;
template std::vector<InterestRateCurveId> Container::ids() const;
template std::vector<HazardRateCurveId  > Container::ids() const;
template std::vector<S3ModelId          > Container::ids() const;
