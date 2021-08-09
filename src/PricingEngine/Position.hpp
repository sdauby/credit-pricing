#pragma once

#include "Container/IdTypes/InstrumentId.hpp"

struct Position {
    double notional = 0.0;
    InstrumentId instrument;
};
