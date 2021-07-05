#pragma once

#include <tuple>
#include <map>
#include <vector>
#include "Container/Container.hpp"
#include "Container/VariantId.hpp"
#include "Elaboration/ElaboratorFactory.hpp"
#include "Elaboration/IdDag.hpp"

// Elaborate a container containing the initial Ids and all
// the Ids required for their elaboration, and so on recursively.
// The elaborator factory is used to create elaborators for the
// individual ids.
// Return the container and the directed acyclical graph of requests.
// The latter structure consists of a map from an Id to the Ids
// it requests.

std::tuple<Container,IdDag>
elaborateContainer(const std::vector<VariantId>& initialIds,
                   const ElaboratorGeneralFactory& factory);
