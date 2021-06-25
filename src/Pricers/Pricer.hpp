#pragma once
#include <vector>
#include <map>
#include "Core/Currency.hpp"
#include "ModelFactory/ModelId.hpp"
#include "Container/InstrumentId.hpp"

class Container;

struct PV {
    double value = {};
    Currency ccy = {};
};

class Pricer {
public:
    virtual ~Pricer() = default;
    virtual std::vector<ModelId> requiredModels() const = 0;
    virtual std::map<InstrumentId,PV> pvs(const Container& container) const = 0;
};
