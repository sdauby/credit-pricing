#pragma once
#include <optional>
class Instrument;

enum class PricerKind { IR, S3 };

class PricingConfiguration final {
public:
    PricingConfiguration(PricerKind pricerKind);
    std::optional<PricerKind> pricerKind(const Instrument& instrument) const;
private:
    PricerKind pricerKind_;
};
