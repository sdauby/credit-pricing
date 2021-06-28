#include "UpdateMutation.hpp"
#include <set>
#include "Container/Container.hpp"
#include "Container/ContainerDag.hpp"
#include "Factory/Factory.hpp"
#include "Factory/FactoryTypes.hpp"

UpdateMutation::UpdateMutation(const std::vector<VariantId>& dirtyIds,
                               const ContainerDag& updateDag,
                               const Factory& factory) :
    dirtyIds_(dirtyIds),
    updateDag_(updateDag),
    factory_(factory)
{}

std::unique_ptr<Container> UpdateMutation::apply(const Container& container) const
{
    auto overlay = std::make_unique<Container>(container);
        
    auto dirtyIds = std::set<VariantId>(dirtyIds_.cbegin(),dirtyIds_.cend());
    auto isDirty = [&dirtyIds] (const auto& id) { return dirtyIds.contains(id); };
        
    auto currentLayer = std::set<VariantId>();
    auto nextLayer = std::set<VariantId>(updateDag_.roots.cbegin(),updateDag_.roots.cend());

    while (!nextLayer.empty()) {
        currentLayer.swap(nextLayer);
        nextLayer.clear();
        for (const auto& id : currentLayer) {
            if (const auto i = updateDag_.precedents.find(id); i != updateDag_.precedents.end()) {
                const auto& precedents = i->second;
                if (std::any_of(precedents.cbegin(),precedents.cend(),isDirty)) {
                    std::visit( [&] (const auto& id) { overlay->set( id, factory_.make(id,*overlay) ); }, id );
                    dirtyIds.insert(id);
                }
            }
            if (const auto i = updateDag_.dependents.find(id); i != updateDag_.dependents.end()) {
                const auto& dependents = i->second;
                nextLayer.insert(dependents.cbegin(),dependents.cend());
            }
        }
    }
    return overlay;
}
