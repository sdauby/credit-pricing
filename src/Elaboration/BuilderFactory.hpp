#pragma once

#include "Elaboration/Builder.hpp"

#include <memory>

template<typename IdT>
using BuilderFactory = std::function< std::unique_ptr<Builder<IdT>> (const IdT&) >;

class BuilderGeneralFactory final {
public:
    template<typename IdT> void setFactory(BuilderFactory<IdT> factory)
    {
        auto& factory_ = std::get<BuilderFactory<IdT>>(factories_);
        factory_ = factory;
    }

    template<typename IdT> std::unique_ptr<Builder<IdT>> make(const IdT& id) const
    {
        const auto & factory_ = std::get<BuilderFactory<IdT>>(factories_);
        return factory_(id);
    }

private:
    template<typename... IdTs> static std::tuple<BuilderFactory<IdTs>...> auxBuilderFactoryTuple(typelist<IdTs...>);
    using BuilderFactoryTuple = decltype(auxBuilderFactoryTuple(IdTypes{}));

    BuilderFactoryTuple factories_;
};
