#pragma once
#include <vector>
#include <map>
#include "Core/Currency.hpp"
#include "ModelContainer/ModelId.hpp"
#include "Instruments/Portfolio.hpp"

using PvResult = std::map<InstrumentId,std::pair<double,Currency>>;
class ModelContainer;

class Pricer {
public:
    virtual ~Pricer() = default;
    virtual std::vector<ModelId> requiredModels() const = 0;
    virtual PvResult pvs(const ModelContainer& modelContainer) const = 0;
};
