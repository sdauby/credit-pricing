#include "Elaboration.hpp"
#include <set>
#include <tuple>
#include <iostream>

#include "ObjectTypes.hpp"

namespace {

std::unique_ptr<BuilderBase> makeBuilder(const BuilderGeneralFactory& factory,
                                         const VariantId& id)
{
    return std::visit(
        [&] (const auto& id) { 
            return std::unique_ptr<BuilderBase>(factory.make(id));
        },
        id);
}

void populate(const VariantId& id, BuilderBase& builder, Container& container) 
{
    std::visit(
        [&] (const auto& id) {
            using IdType = std::remove_cvref_t<decltype(id)>;
            auto& builder_ = dynamic_cast<Builder<IdType>&>(builder);
            container.set(id,builder_.getObject(container));
        },
        id
    );
}

struct BuildState {
    std::vector<std::vector<VariantId>> requestBatches; // successive request batches
    std::unique_ptr<BuilderBase> builder; // made empty when building is complete and built object retrieved
};

}

std::tuple<Container,IdDag> 
elaborateContainer(const std::vector<VariantId>& initialIds,
                   const BuilderGeneralFactory& factory)
{
    Container container;
    std::map<VariantId,BuildState> states;

    const std::function<void(const VariantId& id)> includeId = [&] (const VariantId& id) {
        if (states.contains(id))
            return;
        auto builder = makeBuilder(factory,id);
        auto &state = states[id];
        auto batch = builder->getRequestBatch(Container{});
        for (const auto& request : batch)
            includeId(request);
        state.requestBatches.push_back(std::move(batch));
        state.builder = std::move(builder);
    };

    for (const auto& id : initialIds)
        includeId(id);

    while (true) {
        auto allIdsPopulated = true;
        for (auto& [id,state] : states) {
            if (!state.builder)
                continue;

            const auto& currentBatch = state.requestBatches.back();

            if (currentBatch.empty()) {
                populate(id,*state.builder,container);
                state.builder.reset();
                continue;
            }

            allIdsPopulated = false;
            
            if (std::all_of( currentBatch.cbegin(), currentBatch.cend(),
                             [&] (const auto& request) { return !states[request].builder; } ) ) {
                auto nextBatch = state.builder->getRequestBatch(container);
                for (const auto& request : nextBatch)
                    includeId(request);
                state.requestBatches.push_back(std::move(nextBatch));
            }
        }
        if (allIdsPopulated)
            break;
    }

    IdDag requestDag;
    for (const auto& [id,state] : states) {
        auto& requests = requestDag[id];
        for (const auto& batch : state.requestBatches)
            requests.insert(requests.end(),batch.cbegin(),batch.cend());
    }

    return {std::move(container),std::move(requestDag)};
}
