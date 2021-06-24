#pragma once
#include <map>
#include "Container/Container.hpp"
#include "Container/InstrumentId.hpp"

class Instrument;

using PositionId = unsigned;

struct Position {
    double notional = 0.0;
    InstrumentId instrument;
};

using PositionMap = std::map<PositionId,Position>;

struct Portfolio {
    Container instruments;
    PositionMap positions;
};
