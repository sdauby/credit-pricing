#pragma once
#include <vector>
#include <map>
#include "Core/Currency.hpp"
#include "Container/VariantId.hpp"

class Container;

struct PV {
    double value = {};
    Currency ccy = {};
};

class Pricer {
public:
    virtual ~Pricer() = default;
    virtual std::vector<VariantId> precedents() const = 0;
    virtual std::map<InstrumentId,PV> pvs(const Container& container) const = 0;
};
