#pragma once

class Pricer;

struct PricerId {
    using ObjectType = Pricer;
    size_t k = {};
};

inline bool operator<(const PricerId& x, 
                      const PricerId& y)
{
    return x.k < y.k;
}

inline bool operator==(const PricerId& x, const PricerId& y)
{
    return !(x<y || y<x);
}