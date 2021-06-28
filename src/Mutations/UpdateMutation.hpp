#pragma once

#include "Mutation.hpp"
#include "Container/VariantId.hpp"
#include <vector>

class ContainerDag;
class Factory;

class UpdateMutation : public Mutation {
public:
    UpdateMutation(const std::vector<VariantId>& dirtyIds,
                   const ContainerDag& updateDag,
                   const Factory& factory);
    std::unique_ptr<Container> apply(const Container& container) const override;
private:
    const std::vector<VariantId>& dirtyIds_;
    const ContainerDag& updateDag_;
    const Factory& factory_;
};
