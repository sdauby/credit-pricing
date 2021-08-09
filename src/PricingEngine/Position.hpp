#pragma once

#include "Container/Ids/InstrumentId.hpp"

struct Position {
    double notional = 0.0;
    InstrumentId instrument;
};
