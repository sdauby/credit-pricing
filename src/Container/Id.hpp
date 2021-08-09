#pragma once

#include "Container/Ids/PricerId.hpp"
#include "Container/Ids/InstrumentId.hpp"
#include "Container/Ids/HazardRateCurveId.hpp"
#include "Container/Ids/InterestRateCurveId.hpp"
#include "Container/Ids/S3ModelId.hpp"

template<typename T>
concept Id = 
    requires (T x) 
    { 
        typename T::ObjectType;
        x < x;
        to_string(x);
    };

template<Id IdT>
using Object = typename IdT::ObjectType;

namespace detail {

    template<Id...> struct IdTypeList {};

    using AllIdTypes = IdTypeList<
        PricerId           ,
        InstrumentId       ,
        InterestRateCurveId,
        HazardRateCurveId  ,
        S3ModelId          
    >;

    template<template<Id...> typename T, typename IdTypeList> 
    struct Aux;

    template<template<Id...> typename T, Id... IdTs> 
    struct Aux<T, IdTypeList<IdTs...>> { using type = T<IdTs...>; };

    template <template<Id...> typename T>
    using WithAllIdTypes = typename Aux<T, AllIdTypes>::type;

}

// Instantiate template T with all Id types.
template <template<Id...> typename T>
using WithAllIdTypes = detail::WithAllIdTypes<T>;

