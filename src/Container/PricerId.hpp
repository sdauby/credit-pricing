#pragma once

class Pricer;
enum class PricerKind;

struct PricerId {
    using ObjectType = Pricer;
    PricerKind kind = {};
};

inline bool operator<(const PricerId& x, 
                      const PricerId& y)
{
    return x.kind < y.kind;
}

inline bool operator==(const PricerId& x, const PricerId& y)
{
    return !(x<y || y<x);
}