#include "PricingConfiguration.hpp"
#include "Instruments/Instrument.hpp"
#include "Instruments/InstrumentKind.hpp"

PricingConfiguration::PricingConfiguration(PricerKind preferredKind) : preferredKind_(preferredKind) {}

PricerKind PricingConfiguration::pricerKind(Instrument const& instrument) const
{
    switch (instrument.kind()) {
        case InstrumentKind::FixedCouponBond: 
            return preferredKind_;
        case InstrumentKind::FloatingCouponBond:
            return preferredKind_;
        case InstrumentKind::Cds:
            return PricerKind::S3;
        case InstrumentKind::IRSwap:
            return PricerKind::IR;
    }
}
