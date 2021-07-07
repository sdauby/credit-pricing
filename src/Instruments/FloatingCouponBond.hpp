#pragma once

#include "Core/Currency.hpp"
#include "Core/Issuer.hpp"
#include "Core/Date.hpp"
#include "InstrumentKind.hpp"
#include "InstrumentImpl.hpp"

#include <vector>

struct FloatingCouponBondData {
    Currency ccy = {};
    Issuer issuer;
    std::vector<Date> t; // period dates
    double s; // spread
};

using FloatingCouponBond = InstrumentImpl<InstrumentKind::FloatingCouponBond,FloatingCouponBondData>;
