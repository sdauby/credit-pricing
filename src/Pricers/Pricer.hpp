#pragma once

#include "Core/Currency.hpp"
#include "Container/VariantId.hpp"

#include <vector>
#include <map>

class Container;

struct PV {
    double value = {};
    Currency ccy = {};
};

class Pricer {
public:
    virtual ~Pricer() = default;
    virtual std::vector<VariantId> requests() const = 0;
    virtual std::map<InstrumentId,PV> pvs(const Container& container) const = 0;
};
