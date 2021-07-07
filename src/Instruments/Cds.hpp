#pragma once

#include "Core/Currency.hpp"
#include "Core/Issuer.hpp"
#include "Core/Date.hpp"
#include "InstrumentKind.hpp"
#include "InstrumentImpl.hpp"

#include <vector>

struct CdsData {
    Currency ccy = {};
    Issuer refEntity;
    std::vector<Date> t; // period dates
    double s = {}; // spread (paid)
};

using Cds = InstrumentImpl<InstrumentKind::Cds,CdsData>;
