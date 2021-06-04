#include "PricingConfiguration.hpp"
#include "Instruments/Instrument.hpp"
#include "Instruments/InstrumentKind.hpp"

PricingConfiguration::PricingConfiguration(PricerKind pricerKind) : pricerKind_(pricerKind) {}

std::optional<PricerKind> PricingConfiguration::pricerKind(Instrument const& instrument) const
{
    switch (instrument.kind()) {
        case InstrumentKind::FixedCouponBond: 
            return pricerKind_;
        case InstrumentKind::FloatingCouponBond:
            return pricerKind_;
        case InstrumentKind::Cds:
            switch (pricerKind_) {
                case PricerKind::IR: return std::nullopt;
                case PricerKind::S3: return pricerKind_;
            }
        case InstrumentKind::IRSwap:
            switch (pricerKind_) {
                case PricerKind::IR: return pricerKind_;
                case PricerKind::S3: return std::nullopt;
            }
    }
}
