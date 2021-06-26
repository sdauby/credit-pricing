#pragma once
#include "Container.hpp"
#include "Id.hpp"

template <typename IdT>
using Updater = std::function<void(std::unique_ptr<Object<IdT>>&, // object to update
                                   const std::vector<VariantId>&, // dirty precedents
                                   const Container& // container
                                   )>;

class ContainerDAG {
public:
    template<typename IdT>
    std::vector<VariantId> precedents(IdT& id);
    
    template<typename IdT>
    Updater<IdT> updater(const IdT& id);
};
