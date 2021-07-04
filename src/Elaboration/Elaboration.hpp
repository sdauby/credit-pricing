#pragma once

#include <tuple>
#include <map>
#include <vector>
#include "Container/Container.hpp"
#include "Container/VariantId.hpp"
#include "Elaboration/ElaboratorFactory.hpp"

// Elaborate a container containing the initial Ids and all
// the Ids required for their elaboration, and so on recursively.
// The elaborator factory is used to create elaborators for the
// individual ids.
std::tuple<Container,std::map<VariantId,std::vector<VariantId>>> 
elaborateContainer(const std::vector<VariantId>& initialIds,
                   const ElaboratorGeneralFactory& factory);
