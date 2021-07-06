#pragma once

#include <vector>
#include <memory>
#include "Container/Container.hpp"
#include "Container/VariantId.hpp"

class BuilderBase {
public:
    virtual ~BuilderBase() = default;
    BuilderBase() = default;
    BuilderBase(const BuilderBase&) = delete;
    BuilderBase& operator=(const BuilderBase&) = delete;

    virtual std::vector<VariantId> getRequestBatch(const Container& container) = 0;
};

template<typename IdT>
class Builder : public BuilderBase {
public:
    virtual std::unique_ptr<Object<IdT>> getObject(const Container& container) = 0;
};
