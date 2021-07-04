#pragma once

#include "ElaboratorFactory.hpp"

class InterestRateCurveElaborator : public Elaborator<InterestRateCurveId> {
public:
    InterestRateCurveElaborator(const InterestRateCurveId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<InterestRateCurve> make(const Container& container) override;

    InterestRateCurveId id_;
};
