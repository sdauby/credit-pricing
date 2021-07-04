#pragma once

#include "ElaboratorFactory.hpp"

class HazardRateCurveElaborator : public Elaborator<HazardRateCurveId> {
public:
    HazardRateCurveElaborator(const HazardRateCurveId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<HazardRateCurve> make(const Container& container) override;

    HazardRateCurveId id_;
};
