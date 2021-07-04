#pragma once

#include <vector>
#include <memory>
#include "Container/Container.hpp"
#include "Container/VariantId.hpp"

class ElaboratorBase {
public:
    virtual ~ElaboratorBase() = default;
    ElaboratorBase() = default;
    ElaboratorBase(const ElaboratorBase&) = delete;
    ElaboratorBase& operator=(const ElaboratorBase&) = delete;

    virtual std::vector<VariantId> getRequestBatch(const Container& container) = 0;
};

template<typename IdT>
class Elaborator : public ElaboratorBase {
public:
    virtual std::unique_ptr<Object<IdT>> make(const Container& container) = 0;
};
