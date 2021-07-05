#include "UpdatePropagation.hpp"
#include "Elaboration/ObjectTypes.hpp"
#include <set>

std::unique_ptr<Container> propagateUpdate(const Container& container,
                                           const std::vector<VariantId>& initialIds,
                                           const IdDagAux& idDag,
                                           const ElaboratorGeneralFactory& factory)
{
    auto overlay = std::make_unique<Container>(container);
        
    auto dirty = std::set<VariantId>(initialIds.cbegin(),initialIds.cend());
    const auto isDirty = [&] (const auto& id) { return dirty.contains(id); };
    
    const auto update = [&] (const VariantId& id) {
        std::visit( [&] (const auto& id) -> void {
            const auto elaborator = factory.make(id);
            while (!elaborator->getRequestBatch(*overlay).empty()) {}
            auto newObject = elaborator->make(*overlay);
            overlay->set(id,std::move(newObject));
        }, id);
    };

    auto currentLayer = std::set<VariantId>{};
    auto nextLayer = std::set<VariantId>(idDag.leaves.cbegin(),idDag.leaves.cend());

    while (!nextLayer.empty()) {
        currentLayer.swap(nextLayer);
        nextLayer.clear();
        for (const auto& id : currentLayer) {
            const auto& precedents = idDag.direct.at(id);
            if (std::any_of(precedents.cbegin(),precedents.cend(),isDirty)) {
                update(id);
                dirty.insert(id);
            }
            const auto& dependents = idDag.inverse.at(id);
            nextLayer.insert(dependents.cbegin(),dependents.cend());
        }
    }
    return overlay;
}
