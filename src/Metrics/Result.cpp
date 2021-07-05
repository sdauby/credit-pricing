#include "Result.hpp"
#include "Core/overloaded.hpp"

std::vector<std::string> toStrings(const ResultKey& key) 
{
    return std::visit(overloaded {
        [] (const PVKey& key) {
            return std::vector<std::string>{"PV",name(key.ccy)};
        },
        [] (const IRDeltaKey& key) {
            return std::vector<std::string>{"IRDelta", name(key.curveId.ccy), name(key.ccy)};
        }
    }, key);
}


bool operator< (const PVKey& x, const PVKey& y)
{
    return x.ccy < y.ccy;
}


bool operator< (const IRDeltaKey& x, const IRDeltaKey& y)
{
    const auto tuple = [](const IRDeltaKey& k) {
        return std::tuple(k.ccy,k.curveId);
    };
    return tuple(x)<tuple(y);
}
