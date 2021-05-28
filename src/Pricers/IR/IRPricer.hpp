#pragma once
#include "Pricers/Pricer.hpp"
#include "ModelContainer/InterestRateCurveId.hpp"

class IRUnitPricer {
public:
    virtual ~IRUnitPricer() = default;
    virtual InterestRateCurveId requiredCurve() const = 0;
    virtual double pv(const InterestRateCurve& curve) const = 0;
};

class IRPricer : public Pricer {
public:
    IRPricer(const InstrumentMap& instruments, const std::vector<InstrumentId>& instrumentIds);
    std::vector<ModelId> requiredModels() const override;    
    PvResult pvs(const ModelContainer& modelContainer) const override;
private:
    std::map<InstrumentId,std::unique_ptr<const IRUnitPricer>> unitPricers_;
};