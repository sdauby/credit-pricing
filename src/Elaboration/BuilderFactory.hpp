#pragma once

#include <memory>
#include "Builder.hpp"

template<typename IdT>
using BuilderFactory = std::function<std::unique_ptr<Builder<IdT>>(const IdT&)>;

class BuilderGeneralFactory final {
public:
    ~BuilderGeneralFactory();
    BuilderGeneralFactory();

    template<typename IdT> void setFactory(BuilderFactory<IdT> factory)
    {
        getFactory<IdT>() = factory;
    }

    template<typename IdT> std::unique_ptr<Builder<IdT>> make(const IdT& id) const
    {
        return getFactory<IdT>()(id);
    }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    template<typename IdT> BuilderFactory<IdT>& getFactory() const;
};
