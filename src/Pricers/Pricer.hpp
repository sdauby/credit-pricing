#pragma once
#include <vector>
#include <map>
#include "Core/Currency.hpp"
#include "ModelContainer/ModelId.hpp"
#include "Instruments/Portfolio.hpp"
#include "Result.hpp"

using ResultMap = std::map<InstrumentId,Result>;
class ModelContainer;

class Pricer {
public:
    virtual ~Pricer() = default;
    virtual std::vector<ModelId> requiredModels() const = 0;
    virtual ResultMap pvs(const ModelContainer& modelContainer) const = 0;
};
