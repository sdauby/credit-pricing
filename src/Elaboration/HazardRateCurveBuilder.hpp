#pragma once

#include "BuilderFactory.hpp"

class HazardRateCurveBuilder : public Builder<HazardRateCurveId> {
public:
    HazardRateCurveBuilder(const HazardRateCurveId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<HazardRateCurve> getObject(const Container& container) override;

    HazardRateCurveId id_;
};
