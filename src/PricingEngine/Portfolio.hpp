#pragma once
#include <map>
#include "Container/Container.hpp"
#include "Container/PositionId.hpp"
#include "Container/InstrumentId.hpp"
#include "Elaboration/InstrumentBuilder.hpp"

struct Portfolio {
    std::map<PositionId,Position> positions;
    InstrumentFactory makeInstrument;
};
