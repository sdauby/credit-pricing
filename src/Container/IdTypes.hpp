#pragma once

// The IdTypes types are one-to-one with the ObjectTypes types.

#include <variant>

#include "Container/PricerId.hpp"
#include "Container/InstrumentId.hpp"
#include "Container/HazardRateCurveId.hpp"
#include "Container/InterestRateCurveId.hpp"
#include "Container/S3ModelId.hpp"

namespace detail {

    template<typename... Ts> struct typelist {};

    using IdTypes = typelist<
        PricerId           ,
        InstrumentId       ,
        InterestRateCurveId,
        HazardRateCurveId  ,
        S3ModelId
    >;

    template<template<typename...> typename T, typename typelist> 
    struct Aux;

    template<template<typename...> typename T, typename... IdTs> 
    struct Aux<T, typelist<IdTs...>> { using type = T<IdTs...>; };

    template <template<typename...> typename T>
    using ApplyToIdTypes = typename Aux<T, IdTypes>::type;

}

template <template<typename...> typename T>
using ApplyToIdTypes = detail::ApplyToIdTypes<T>;

template<typename IdT>
using Object = typename IdT::ObjectType;
