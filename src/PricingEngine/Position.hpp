#pragma once

#include "Container/InstrumentId.hpp"

struct Position {
    double notional = 0.0;
    InstrumentId instrument;
};
