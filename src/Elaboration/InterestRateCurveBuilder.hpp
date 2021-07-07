#pragma once

#include "Elaboration/Builder.hpp"

class InterestRateCurveBuilder : public Builder<InterestRateCurveId> {
public:
    InterestRateCurveBuilder(const InterestRateCurveId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<InterestRateCurve> getObject(const Container&) override;

    InterestRateCurveId id_;
};
