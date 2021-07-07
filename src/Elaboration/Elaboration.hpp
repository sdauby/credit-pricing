#pragma once

#include "Container/Container.hpp"
#include "Container/VariantId.hpp"
#include "Elaboration/BuilderFactory.hpp"
#include "Elaboration/IdDag.hpp"

#include <tuple>
#include <vector>

// Elaborate a container by building the initial ids, the ids they request, 
// and so on recursively.
// Return the elaborated container and the directed acyclical graph of requests.
// The latter structure consists of a map from each id in the
// container to the ids requested by its builder.

std::tuple<Container,IdDag>
elaborateContainer(const std::vector<VariantId>& initialIds,
                   const BuilderGeneralFactory& factory);
