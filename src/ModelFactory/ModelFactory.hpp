#pragma once
#include "ModelContainer/ModelContainer.hpp"
#include "ModelContainer/ModelId.hpp"

class Model;

class ModelFactory {
public:
    std::vector<ModelId> requiredModels(const ModelId& modelId) const;
    std::unique_ptr<Model> make(const ModelId& modelId, const ModelContainer& modelContainer) const;
};

void populate(ModelContainer& modelContainer,
              const ModelId& modelId,
              const ModelFactory& modelFactory);