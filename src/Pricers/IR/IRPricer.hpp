#pragma once

#include "Pricers/Pricer.hpp"

class IRUnitPricer {
public:
    virtual ~IRUnitPricer() = default;
    virtual InterestRateCurveId requiredCurve() const = 0;
    virtual double pv(const InterestRateCurve& curve) const = 0;
};

class IRPricer : public Pricer {
public:
    IRPricer(const Container& container, const std::vector<InstrumentId>& instrumentIds);
    std::vector<VariantId> requests() const override;    
    std::map<InstrumentId,PV> pvs(const Container& container) const override;
private:
    std::map<InstrumentId,std::unique_ptr<const IRUnitPricer>> unitPricers_;
};