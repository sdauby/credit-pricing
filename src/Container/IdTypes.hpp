#pragma once

// The IdT types are one-to-one with the ObjectT types.

#include "Container/PricerId.hpp"
#include "Container/InstrumentId.hpp"
#include "Container/HazardRateCurveId.hpp"
#include "Container/InterestRateCurveId.hpp"
#include "Container/S3ModelId.hpp"

template<template<class> class Unit>
using IdTypesTuple = std::tuple<
    Unit<PricerId           >,
    Unit<InstrumentId       >,
    Unit<InterestRateCurveId>,
    Unit<HazardRateCurveId  >,
    Unit<S3ModelId          >
>;

template<typename IdT>
using Object = typename IdT::ObjectType;
