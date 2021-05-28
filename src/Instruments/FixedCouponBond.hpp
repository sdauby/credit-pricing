#pragma once
#include <vector>
#include "Core/Currency.hpp"
#include "Core/Issuer.hpp"
#include "Core/Date.hpp"
#include "InstrumentKind.hpp"
#include "InstrumentImpl.hpp"

struct FixedCouponBondData {
    Currency ccy = {};
    Issuer issuer;
    std::vector<Date> t; // period dates
    double c = {}; // coupon rate
};

using FixedCouponBond = InstrumentImpl<InstrumentKind::FixedCouponBond,FixedCouponBondData>;
