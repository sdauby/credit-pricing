#pragma once

#include "Container/IdTypes.hpp"

#include <string>
#include <variant>

using VariantId = std::variant<
    InstrumentId,
    PricerId,
    InterestRateCurveId,
    HazardRateCurveId,
    S3ModelId
>;

std::string to_string(const VariantId& id);
