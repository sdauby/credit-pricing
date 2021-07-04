#pragma once

#include <memory>
#include "Elaborator.hpp"

template<typename IdT>
using ElaboratorFactory = std::function<std::unique_ptr<Elaborator<IdT>>(const IdT&)>;

class ElaboratorGeneralFactory final {
public:
    ~ElaboratorGeneralFactory();
    ElaboratorGeneralFactory();

    template<typename IdT> void setFactory(ElaboratorFactory<IdT> factory)
    {
        getFactory<IdT>() = factory;
    }

    template<typename IdT> std::unique_ptr<Elaborator<IdT>> make(const IdT& id) const
    {
        return getFactory<IdT>()(id);
    }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    template<typename IdT> ElaboratorFactory<IdT>& getFactory() const;
};
