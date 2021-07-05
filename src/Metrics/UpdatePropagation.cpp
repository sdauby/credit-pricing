#include "UpdatePropagation.hpp"
#include "Elaboration/ObjectTypes.hpp"
#include <set>

std::tuple<
    std::unique_ptr<Container>,
    std::vector<VariantId>
    >
propagateUpdate(const Container& container,
                const std::vector<VariantId>& initialIds,
                const IdDagAux& idDag,
                const UpdateFunction& update)
{
    auto overlay = std::make_unique<Container>(container);
        
    auto dirty = std::set<VariantId>(initialIds.cbegin(),initialIds.cend());
    const auto isDirty = [&] (const auto& id) { return dirty.contains(id); };
    
    auto currentLayer = std::set<VariantId>{};
    auto nextLayer = std::set<VariantId>(idDag.leaves.cbegin(),idDag.leaves.cend());
    auto dirtyPrecedents = std::vector<VariantId>{};

    while (!nextLayer.empty()) {
        currentLayer.swap(nextLayer);
        nextLayer.clear();
        for (const auto& id : currentLayer) {
            const auto& precedents = idDag.direct.at(id);
            dirtyPrecedents.clear();
            std::copy_if(precedents.cbegin(),precedents.cend(),std::back_inserter(dirtyPrecedents),isDirty);
            if (!dirtyPrecedents.empty()) {
                update(*overlay,id,dirtyPrecedents);
                dirty.insert(id);
            }
            const auto& dependents = idDag.inverse.at(id);
            nextLayer.insert(dependents.cbegin(),dependents.cend());
        }
    }
    return {std::move(overlay),std::vector<VariantId>(dirty.cbegin(),dirty.cend())};
}
