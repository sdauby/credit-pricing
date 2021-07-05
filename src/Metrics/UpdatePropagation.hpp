#pragma once

#include "Container/Container.hpp"
#include "Container/VariantId.hpp"
#include "Elaboration/IdDag.hpp"
#include "Elaboration/ElaboratorFactory.hpp"
#include <memory>
#include <vector>

std::unique_ptr<Container> propagateUpdate(const Container& container,
                                           const std::vector<VariantId>& initialIds,
                                           const IdDagAux& idDag,
                                           const ElaboratorGeneralFactory& factory);
