#pragma once

#include "Mutation.hpp"
#include "Container/VariantId.hpp"
#include "Elaboration/IdDag.hpp"
#include <vector>

class ElaboratorGeneralFactory;

class UpdateMutation : public Mutation {
public:
    UpdateMutation(const std::vector<VariantId>& dirtyIds,
                   const IdDagAux& requests,
                   const ElaboratorGeneralFactory& factory);
    std::unique_ptr<Container> apply(const Container& container) const override;
private:
    const std::vector<VariantId>& dirtyIds_;
    const IdDagAux& requests_;
    const ElaboratorGeneralFactory& factory_;
};
