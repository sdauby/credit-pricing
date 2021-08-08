#pragma once

// The IdTypes types are one-to-one with the ObjectTypes types.

#include <variant>

#include "Container/PricerId.hpp"
#include "Container/InstrumentId.hpp"
#include "Container/HazardRateCurveId.hpp"
#include "Container/InterestRateCurveId.hpp"
#include "Container/S3ModelId.hpp"

template<typename... Ts> struct typelist {};

using IdTypes = typelist<
    PricerId           ,
    InstrumentId       ,
    InterestRateCurveId,
    HazardRateCurveId  ,
    S3ModelId
>;

template<typename IdT>
using Object = typename IdT::ObjectType;
