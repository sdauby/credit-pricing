#pragma once

enum class InstrumentKind {
    FixedCouponBond,
    FloatingCouponBond,
    Cds,
};

constexpr const char* name(InstrumentKind kind)
{
    switch (kind) {
        using IK = InstrumentKind;
        case IK::FixedCouponBond   : return "FixedCouponBond";
        case IK::FloatingCouponBond: return "FloatingCouponBond";
        case IK::Cds               : return "Cds";
    }
}
