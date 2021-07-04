#pragma once
#include <variant>
#include "PositionId.hpp"
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

std::string to_string(const VariantId& id);