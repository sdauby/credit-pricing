#pragma once

#include "Container/Container.hpp"
#include "Container/VariantId.hpp"
#include "Elaboration/IdDag.hpp"
#include "Elaboration/ElaboratorFactory.hpp"
#include <memory>
#include <vector>

using UpdateFunction = std::function<
    void(Container& container, const VariantId& id, const std::vector<VariantId>& dirtyPrecedents)
    >;

std::tuple<
    std::unique_ptr<Container>,
    std::vector<VariantId> // dirty ids
    >
propagateUpdate(const Container& container,
                const std::vector<VariantId>& initialIds,
                const IdDagAux& idDag,
                const UpdateFunction& update);
