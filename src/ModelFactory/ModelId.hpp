#pragma once
#include <variant>
#include "Container/InterestRateCurveId.hpp"
#include "Container/HazardRateCurveId.hpp"
#include "Container/S3ModelId.hpp"

using ModelId = std::variant<
    InterestRateCurveId,
    HazardRateCurveId,
    S3ModelId
>;
