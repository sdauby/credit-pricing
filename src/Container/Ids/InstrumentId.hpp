#pragma once

#include <string>

class Instrument;

struct InstrumentId {
    using ObjectType = Instrument;
    unsigned k = {};
};

inline bool operator<(const InstrumentId& x, 
                      const InstrumentId& y)
{
    return x.k < y.k;
}

inline bool operator==(const InstrumentId& x, const InstrumentId& y)
{
    return x.k == y.k;
}

inline std::string to_string(const InstrumentId& id)
{
    return "Instrument{" + std::to_string(id.k) + "}";    
}
