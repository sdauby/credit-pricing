#pragma once
class Instrument;

enum class PricerKind { IR, S3 };

constexpr const char* name(PricerKind kind)
{
    switch (kind) {
        case PricerKind::IR: return "IR";
        case PricerKind::S3: return "S3";
    }
}


class PricingConfiguration final {
public:
    PricingConfiguration(PricerKind preferredKind);
    PricerKind pricerKind(const Instrument& instrument) const;
private:
    PricerKind preferredKind_;
};
