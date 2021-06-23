#pragma once
#include <vector>
#include <map>
#include "Core/Currency.hpp"
#include "ModelContainer/ModelId.hpp"
#include "Instruments/Portfolio.hpp"

class ModelContainer;

struct PV {
    double value = {};
    Currency ccy = {};
};

class Pricer {
public:
    virtual ~Pricer() = default;
    virtual std::vector<ModelId> requiredModels() const = 0;
    virtual std::map<InstrumentId,PV> pvs(const ModelContainer& modelContainer) const = 0;
};
