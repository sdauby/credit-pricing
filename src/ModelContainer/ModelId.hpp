#pragma once
#include <variant>
#include "InterestRateCurveId.hpp"
#include "HazardRateCurveId.hpp"
#include "S3ModelId.hpp"

using ModelId = std::variant<
    InterestRateCurveId,
    HazardRateCurveId,
    S3ModelId
>;