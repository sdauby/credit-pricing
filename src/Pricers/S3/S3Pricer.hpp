#pragma once

#include "Pricers/Pricer.hpp"

class S3UnitPricer {
public:
    virtual ~S3UnitPricer() = default;
    virtual S3ModelId modelId() const = 0;
    virtual double pv(const S3Model& model) const = 0;
};

class S3Pricer : public Pricer {
public:
    S3Pricer(const Container& container, const std::vector<InstrumentId>& instrumentIds);
    std::vector<VariantId> requests() const override;
    std::map<InstrumentId,PV> pvs(const Container& container) const override;
private:
    std::map<InstrumentId,std::unique_ptr<const S3UnitPricer>> unitPricers_;
    std::map<InstrumentId,S3ModelId> s3ModelIds_;
};
