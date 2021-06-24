#pragma once

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
    return !(x<y || y<x);
}