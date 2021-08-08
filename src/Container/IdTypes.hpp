#pragma once

#include "Container/PricerId.hpp"
#include "Container/InstrumentId.hpp"
#include "Container/HazardRateCurveId.hpp"
#include "Container/InterestRateCurveId.hpp"
#include "Container/S3ModelId.hpp"

template<typename T>
concept IdType = 
    requires { typename T::ObjectType; }
    && requires (T x) 
    { 
        x < x;
        to_string_(x);
    };

namespace detail {

    template<IdType... Ts> struct typelist {};

    using IdTypes = typelist<
        PricerId           ,
        InstrumentId       ,
        InterestRateCurveId,
        HazardRateCurveId  ,
        S3ModelId          
    >;

    template<template<IdType...> typename T, typename typelist> 
    struct Aux;

    template<template<IdType...> typename T, IdType... IdTs> 
    struct Aux<T, typelist<IdTs...>> { using type = T<IdTs...>; };

    template <template<IdType...> typename T>
    using ApplyToIdTypes = typename Aux<T, IdTypes>::type;

}

template <template<IdType...> typename T>
using ApplyToIdTypes = detail::ApplyToIdTypes<T>;

template<typename IdT>
using Object = typename IdT::ObjectType;
