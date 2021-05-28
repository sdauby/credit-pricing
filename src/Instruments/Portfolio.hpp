#pragma once
#include <map>

class Instrument;

using PositionId = unsigned;
using InstrumentId = unsigned;

struct Position {
    double notional = 0.0;
    InstrumentId instrument;
};

using InstrumentMap = std::map<InstrumentId,std::unique_ptr<Instrument>>;
using PositionMap = std::map<PositionId,Position>;

struct Portfolio {
    InstrumentMap instruments;
    PositionMap positions;
};
