#pragma once

// The IdT types are one-to-one with the ObjectT types.

#include <variant>

#include "Container/PricerId.hpp"
#include "Container/InstrumentId.hpp"
#include "Container/HazardRateCurveId.hpp"
#include "Container/InterestRateCurveId.hpp"
#include "Container/S3ModelId.hpp"

template<template<class> class UnitT>
using IdTypesTuple = std::tuple<
    UnitT<PricerId           >,
    UnitT<InstrumentId       >,
    UnitT<InterestRateCurveId>,
    UnitT<HazardRateCurveId  >,
    UnitT<S3ModelId          >
>;

using VariantId = std::variant<
    PricerId           ,
    InstrumentId       ,
    InterestRateCurveId,
    HazardRateCurveId  ,
    S3ModelId
>;

template<typename IdT>
using Object = typename IdT::ObjectType;
