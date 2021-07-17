#include "Elaboration/Elaboration.hpp"

#include "Container/ObjectTypes.hpp"

#include <set>
#include <tuple>

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
    std::map<VariantId,BuildState> buildingStates;
    std::map<VariantId,BuildState> builtStates;

    std::function<void(const VariantId& id)> include;

    const auto getNextBatch = [&] (const VariantId& id) {
        auto& state = buildingStates.at(id); 
        state.requestBatches.push_back(state.builder->getRequestBatch(container));
        const auto& batch = state.requestBatches.back();
        if (batch.empty()) {
            populate(id,*state.builder,container);
            state.builder.reset();
            builtStates.insert(buildingStates.extract(id));
        }
        else {
            for (const auto& request : batch)
                include(request);
        }
    };

    include = [&] (const VariantId& id) {
        if (buildingStates.contains(id) || builtStates.contains(id))
            return;
        auto &state = buildingStates[id];
        state.builder = makeBuilder(factory,id);
        getNextBatch(id);
    };

    for (const auto& id : initialIds)
        include(id);

    while (!buildingStates.empty()) {
        for (auto i=buildingStates.begin(), e=buildingStates.end(); i!=e;) {
            auto& [id,state] = *i;
            ++i;

            const auto& currentBatch = state.requestBatches.back();            
            if (std::all_of( currentBatch.cbegin(), currentBatch.cend(),
                             [&] (const auto& request) { return builtStates.contains(request); } ) ) {
                getNextBatch(id);
            }
        }
    }

    IdDag requestDag;
    for (const auto& [id,state] : builtStates) {
        auto& requests = requestDag[id];
        for (const auto& batch : state.requestBatches)
            requests.insert(requests.end(),batch.cbegin(),batch.cend());
    }

    return {std::move(container),std::move(requestDag)};
}
