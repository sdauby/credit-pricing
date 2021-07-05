#include "UpdateMutation.hpp"
#include <set>
#include "Container/Container.hpp"
#include "Container/ContainerDag.hpp"
#include "Factory/Factory.hpp"
#include "Factory/FactoryTypes.hpp"
#include "Elaboration/ElaboratorFactory.hpp"

UpdateMutation::UpdateMutation(const std::vector<VariantId>& dirtyIds,
                               const IdDagAux& requests,
                               const ElaboratorGeneralFactory& factory) :
    dirtyIds_(dirtyIds),
    requests_(requests),
    factory_(factory)
{}

std::unique_ptr<Container> UpdateMutation::apply(const Container& container) const
{
    auto overlay = std::make_unique<Container>(container);
        
    auto dirty = std::set<VariantId>(dirtyIds_.cbegin(),dirtyIds_.cend());
    const auto isDirty = [&dirty] (const auto& id) { return dirty.contains(id); };
    
    const auto update = [&] (const VariantId& id) {
        std::visit( [&] (const auto& id) -> void {
            const auto elaborator = factory_.make(id);
            while (!elaborator->getRequestBatch(*overlay).empty()) {}
            auto newObject = elaborator->make(*overlay);
            overlay->set(id,std::move(newObject));
        }, id);
    };

    auto currentLayer = std::set<VariantId>{};
    auto nextLayer = std::set<VariantId>(requests_.leaves.cbegin(),requests_.leaves.cend());

    while (!nextLayer.empty()) {
        currentLayer.swap(nextLayer);
        nextLayer.clear();
        for (const auto& id : currentLayer) {
            const auto& requests = requests_.direct.at(id);
            if (std::any_of(requests.cbegin(),requests.cend(),isDirty)) {
                update(id);
                dirty.insert(id);
            }
            const auto& requestedBy = requests_.inverse.at(id);
            nextLayer.insert(requestedBy.cbegin(),requestedBy.cend());
        }
    }
    return overlay;
}
