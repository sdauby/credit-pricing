#pragma once

#include "Pricers/Pricer.hpp"

class S3UnitPricer {
public:
    virtual ~S3UnitPricer() = default;
    virtual S3ModelId requiredModel() const = 0;
    virtual double pv(const S3Model& model) const = 0;
};

class S3Pricer : public Pricer {
public:
    S3Pricer(const Container& container, const std::vector<InstrumentId>& instrumentIds);
    std::vector<S3ModelId> requiredModels() const;

private:
    std::map<InstrumentId,PV> pvs(const Container& container) const override;

    std::map<InstrumentId,std::unique_ptr<const S3UnitPricer>> unitPricers_;
    std::map<InstrumentId,S3ModelId> s3ModelIds_;
};
