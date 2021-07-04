#pragma once

#include <string>

struct Position;

struct PositionId {
    using ObjectType = Position;
    unsigned k = {};
};

inline bool operator<(const PositionId& x, 
                      const PositionId& y)
{
    return x.k < y.k;
}

inline bool operator==(const PositionId& x, const PositionId& y)
{
    return !(x<y || y<x);
}

inline std::string to_string_(const PositionId& id)
{
    return "PositionId{" + std::to_string(id.k) + "}";    
}
