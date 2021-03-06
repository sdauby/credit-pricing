#pragma once

#include <memory>
#include <vector>
#include <set>

#include "Container/Id.hpp"
#include "Container/Objects.hpp"

class Container final {
public:
    Container() = default;
    Container(Container&& container) = default;
    Container& operator=(Container&& container) = default;
    explicit Container(const Container& baseContainer) : baseContainer_(&baseContainer) {}

    template<Id IdT>
    Object<IdT>* get(const IdT& id) const
    {
        const auto& objectMap = objects<IdT>();
        const auto& i = objectMap.find(id);
        if (i==objectMap.end()) {
            if (baseContainer_)
                return baseContainer_->get(id);
            else
                return nullptr;
        } else {
            return i->second.get();
        }
    }

    template<Id IdT>
    void set(const IdT& id, std::unique_ptr<Object<IdT>>&& object)
    {
        objects<IdT>().emplace( std::pair{ id, std::move(object) } );
    }

    template<Id IdT>
    std::vector<IdT> ids() const
    {
        std::set<IdT> ids;
        if (baseContainer_) {
            const auto baseContainerIds = baseContainer_->ids<IdT>();
            ids.insert(baseContainerIds.cbegin(),baseContainerIds.cend());
        }
        for (const auto& [id, object] : objects<IdT>())
            ids.insert(id);
        return std::vector<IdT>(ids.cbegin(),ids.cend());
    }

private:
    template<Id IdT>
    using ObjectMap = std::map<IdT,std::unique_ptr<Object<IdT>>>;

    template<Id... IdTs> using ObjectMapTupleT = std::tuple<ObjectMap<IdTs>...>;
    using ObjectMapTuple = WithAllIdTypes<ObjectMapTupleT>;

    ObjectMapTuple objects_;
    const Container* baseContainer_ = nullptr;

    template<Id IdT>
    const ObjectMap<IdT>& objects() const { return std::get<ObjectMap<IdT>>(objects_); }

    template<Id IdT>
    ObjectMap<IdT>& objects() { return const_cast<ObjectMap<IdT>&>( std::as_const(*this).objects<IdT>() ); }
};
