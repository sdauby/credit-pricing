#pragma once

#include "Core/Currency.hpp"
#include "Core/Date.hpp"
#include "InstrumentKind.hpp"
#include "InstrumentImpl.hpp"

#include <vector>

struct IRSwapData {
    Currency ccy = {};
    std::vector<Date> t; // period dates
    double K = {}; // fixed rate (received)
};

using IRSwap = InstrumentImpl<InstrumentKind::IRSwap,IRSwapData>;
