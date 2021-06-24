#pragma once
#include "ModelContainer/ModelContainer.hpp"
#include "ModelContainer/ModelId.hpp"

class Model;

class ModelFactory {
public:
    std::vector<ModelId> precedents(const ModelId& modelId) const;
    std::unique_ptr<Model> make(const ModelId& modelId, const Container& modelContainer) const;
};

void populate(Container& modelContainer,
              const ModelId& modelId,
              const ModelFactory& modelFactory);