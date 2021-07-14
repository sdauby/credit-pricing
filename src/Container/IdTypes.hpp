#pragma once

// The IdTypes types are one-to-one with the ObjectTypes types.

#include <variant>

#include "Container/PricerId.hpp"
#include "Container/InstrumentId.hpp"
#include "Container/HazardRateCurveId.hpp"
#include "Container/InterestRateCurveId.hpp"
#include "Container/S3ModelId.hpp"

template<class... IdTypes> struct IdTypesAuxTemplate {
    
    template<template<class> class UnitT>
    using tuple = std::tuple<UnitT<IdTypes>...>;
    
    using variant = std::variant<IdTypes...>;

};

using IdTypesAux = IdTypesAuxTemplate<
    PricerId           ,
    InstrumentId       ,
    InterestRateCurveId,
    HazardRateCurveId  ,
    S3ModelId
>;

template<template<class> class UnitT>
using IdTypesTuple = IdTypesAux::tuple<UnitT>;

using VariantId = IdTypesAux::variant;

template<typename IdT>
using Object = typename IdT::ObjectType;
