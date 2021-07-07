#pragma once

class Instrument;
enum class PricerKind;

class PricingConfiguration final {
public:
    PricingConfiguration(PricerKind preferredKind);
    PricerKind pricerKind(const Instrument& instrument) const;
private:
    PricerKind preferredKind_;
};
