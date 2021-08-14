#pragma once

#include "Pricers/Pricer.hpp"

class PricingConfiguration;

class GeneralPricer : public Pricer {
public:
    GeneralPricer(const Container& container,
                  const std::vector<InstrumentId>& instruments,
                  const PricingConfiguration& config);

    std::vector<PricerId> requiredPricers() const;

private:
    virtual std::map<InstrumentId,PV> pvs(const Container& container) const override;

    std::vector<PricerId> pricers_;
};
