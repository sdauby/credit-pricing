#include "Elaboration.hpp"
#include <set>
#include <tuple>
#include <iostream>

#include "Factory/FactoryTypes.hpp"

namespace {

std::unique_ptr<ElaboratorBase> makeElaborator(const ElaboratorGeneralFactory& factory,
                                               const VariantId& id)
{
    return std::visit(
        [&] (const auto& id) { 
            return std::unique_ptr<ElaboratorBase>(factory.make(id));
        },
        id);
}

void populate(const VariantId& id, ElaboratorBase& elaborator, Container& container) 
{
    std::visit(
        [&] (const auto& id) {
            using IdType = std::remove_cvref_t<decltype(id)>;
            auto& elaborator_ = dynamic_cast<Elaborator<IdType>&>(elaborator);
            container.set(id,elaborator_.make(container));
        },
        id
    );
}

struct ElaborationState {
    std::vector<std::vector<VariantId>> requestBatches; // successive request batches
    std::unique_ptr<ElaboratorBase> elaborator; // made empty when elaboration is complete and container populated
};

}

std::tuple<Container,std::map<VariantId,std::vector<VariantId>>> 
elaborateContainer(const std::vector<VariantId>& initialIds,
                   const ElaboratorGeneralFactory& factory)
{
    Container container;
    std::map<VariantId,ElaborationState> states;

    const std::function<void(const VariantId& id)> includeId = [&] (const VariantId& id) {
        if (states.contains(id))
            return;
        auto elaborator = makeElaborator(factory,id);
        auto &state = states[id];
        auto batch = elaborator->getRequestBatch(Container{});
        for (const auto& request : batch)
            includeId(request);
        state.requestBatches.push_back(std::move(batch));
        state.elaborator = std::move(elaborator);
    };

    for (const auto& id : initialIds)
        includeId(id);

    while (true) {
        auto allIdsPopulated = true;
        for (auto& [id,state] : states) {
            if (!state.elaborator)
                continue;

            const auto& currentBatch = state.requestBatches.back();

            if (currentBatch.empty()) {
                populate(id,*state.elaborator,container);
                state.elaborator.reset();
                continue;
            }

            allIdsPopulated = false;
            
            if (std::all_of( currentBatch.cbegin(), currentBatch.cend(),
                             [&] (const auto& request) { return !states[request].elaborator; } ) ) {
                auto nextBatch = state.elaborator->getRequestBatch(container);
                for (const auto& request : nextBatch)
                    includeId(request);
                state.requestBatches.push_back(std::move(nextBatch));
            }
        }
        if (allIdsPopulated)
            break;
    }

    std::map<VariantId,std::vector<VariantId>> requestDag;
    for (const auto& [id,state] : states) {
        auto& requests = requestDag[id];
        for (const auto& batch : state.requestBatches)
            requests.insert(requests.end(),batch.cbegin(),batch.cend());
    }

    return {std::move(container),std::move(requestDag)};
}
