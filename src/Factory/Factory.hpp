#pragma once
#include <vector>
#include "Container/VariantId.hpp"
#include "Container/Container.hpp"

class PricingConfiguration;

class Factory final {
public: 
    ~Factory();
    explicit Factory(const PricingConfiguration& config);

    template<typename IdT>
    std::vector<VariantId> getPrecedents(const IdT& id, const Container& container) const;

    template<typename IdT>
    std::unique_ptr<Object<IdT>> make(const IdT& id, const Container& container) const;
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
