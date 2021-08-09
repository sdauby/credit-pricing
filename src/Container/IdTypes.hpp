#pragma once

#include "Container/IdTypes/PricerId.hpp"
#include "Container/IdTypes/InstrumentId.hpp"
#include "Container/IdTypes/HazardRateCurveId.hpp"
#include "Container/IdTypes/InterestRateCurveId.hpp"
#include "Container/IdTypes/S3ModelId.hpp"

template<typename T>
concept IdType = 
    requires (T x) 
    { 
        typename T::ObjectType;
        x < x;
        to_string(x);
    };

namespace detail {

    template<IdType...> struct IdTypeList {};

    using AllIdTypes = IdTypeList<
        PricerId           ,
        InstrumentId       ,
        InterestRateCurveId,
        HazardRateCurveId  ,
        S3ModelId          
    >;

    template<template<IdType...> typename T, typename IdTypeList> 
    struct Aux;

    template<template<IdType...> typename T, IdType... IdTs> 
    struct Aux<T, IdTypeList<IdTs...>> { using type = T<IdTs...>; };

    template <template<IdType...> typename T>
    using WithAllIdTypes = typename Aux<T, AllIdTypes>::type;

}

template <template<IdType...> typename T>
using WithAllIdTypes = detail::WithAllIdTypes<T>;

template<IdType IdT>
using Object = typename IdT::ObjectType;
