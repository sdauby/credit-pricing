#pragma once

#include <tuple>
#include <map>
#include <vector>
#include "Container/Container.hpp"
#include "Container/VariantId.hpp"
#include "Elaboration/BuilderFactory.hpp"
#include "Elaboration/IdDag.hpp"

// Elaborate a container by building the objects designated by
// the initial ids, the ids they request, and so on recursively.
// Return the container and the directed acyclical graph of requests.
// The latter structure consists of a map from each id in the
// container to the ids it requests.

std::tuple<Container,IdDag>
elaborateContainer(const std::vector<VariantId>& initialIds,
                   const BuilderGeneralFactory& factory);
