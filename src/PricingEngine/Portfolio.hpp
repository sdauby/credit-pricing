#pragma once

#include "Container/PositionId.hpp"
#include "PricingEngine/Position.hpp"
#include "Elaboration/InstrumentBuilder.hpp"

#include <map>

struct Portfolio {
    std::map<PositionId,Position> positions;
    InstrumentFactory makeInstrument;
};
