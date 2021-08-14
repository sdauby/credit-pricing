#pragma once

#include "Builder.hpp"

class HazardRateCurveBuilder : public Builder<HazardRateCurveId> {
public:
    explicit HazardRateCurveBuilder(const HazardRateCurveId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<HazardRateCurve> getObject(const Container&) override;

    HazardRateCurveId id_;
};
