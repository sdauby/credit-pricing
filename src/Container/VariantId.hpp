#pragma once
#include <variant>
#include "InstrumentId.hpp"
#include "PricerId.hpp"
#include "InterestRateCurveId.hpp"
#include "HazardRateCurveId.hpp"
#include "S3ModelId.hpp"

using VariantId = std::variant<
    InstrumentId,
    PricerId,
    InterestRateCurveId,
    HazardRateCurveId,
    S3ModelId
>;
