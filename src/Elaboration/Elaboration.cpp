#include "Elaboration.hpp"
#include <set>
#include <tuple>
#include <iostream>

#include "Factory/FactoryTypes.hpp"

namespace {

class ElaborationAssistant {
public:
    ElaborationAssistant(Container & container, const ElaboratorGeneralFactory& elaboratorGeneralFactory) :
        container_(container),
        elaboratorGeneralFactory_(elaboratorGeneralFactory)
    {}

    std::unique_ptr<ElaboratorBase> makeElaborator(const VariantId& id)
    {
        return std::visit(
            [&] (const auto& id) { 
                return std::unique_ptr<ElaboratorBase>(elaboratorGeneralFactory_.make(id));
            },
            id);
    }

    void populate(const VariantId& id, ElaboratorBase& elaborator) 
    {
        std::visit(
            [&] (const auto& id) {
                using IdType = std::remove_cvref_t<decltype(id)>;
                auto& elaborator_ = dynamic_cast<Elaborator<IdType>&>(elaborator);
                container_.set(id,elaborator_.make(container_));
            },
            id
        );
    }


    std::map<VariantId,std::vector<VariantId>> requestDag()
    {
        return {}; // to do
        // I think that I could put more state and more validation of the
        // expected call sequence in the assistant: oustanding requests,
        // resolved requests. With that info I can get the precedents DAG too.
    }
private:
    const ElaboratorGeneralFactory& elaboratorGeneralFactory_;
    Container& container_;
};

}

struct ElaborationState {
    std::vector<std::vector<VariantId>> requestBatches; // successive request batches
    std::unique_ptr<ElaboratorBase> elaborator; // made empty when elaboration is complete and container populated
};

std::tuple<Container,std::map<VariantId,std::vector<VariantId>>> 
elaborateContainer(const std::vector<VariantId>& initialIds,
                   const ElaboratorGeneralFactory& factory)
{
    Container container;
    ElaborationAssistant assistant(container,factory);

    std::map<VariantId,ElaborationState> states;

    const std::function<void(const VariantId& id)> includeId = [&] (const VariantId& id) {
        if (states.contains(id))
            return;
        auto elaborator = assistant.makeElaborator(id);
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
                assistant.populate(id,*state.elaborator);
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
    return {std::move(container),assistant.requestDag()};
}
