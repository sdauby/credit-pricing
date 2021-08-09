#pragma once

#include "Elaboration/Builder.hpp"

#include <memory>

template<Id IdT>
using BuilderFactory = std::function< std::unique_ptr<Builder<IdT>> (const IdT&) >;

class BuilderGeneralFactory final {
public:
    template<Id IdT> void setFactory(BuilderFactory<IdT> factory)
    {
        auto& factory_ = std::get<BuilderFactory<IdT>>(factories_);
        factory_ = factory;
    }

    template<Id IdT> std::unique_ptr<Builder<IdT>> make(const IdT& id) const
    {
        const auto& factory_ = std::get<BuilderFactory<IdT>>(factories_);
        return factory_(id);
    }

private:
    template<Id... IdTs> using BuilderFactoryTupleT = std::tuple<BuilderFactory<IdTs>...>;
    using BuilderFactoryTuple = WithAllIdTypes<BuilderFactoryTupleT>;
    BuilderFactoryTuple factories_;
};
