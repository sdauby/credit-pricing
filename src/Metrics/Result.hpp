#pragma once
#include <variant>
#include <map>
#include <vector>
#include <string>
#include "Core/Currency.hpp"
#include "ModelContainer/InterestRateCurveId.hpp"

struct PVKey {
    Currency ccy;
};

bool operator< (const PVKey& x, const PVKey& y);


struct IRDeltaKey {
    Currency ccy;
    InterestRateCurveId curveId;
};

bool operator< (const IRDeltaKey& x, const IRDeltaKey& y);


using ResultKey = std::variant<
    PVKey,
    IRDeltaKey
>;

using Result = std::map<ResultKey,double>;

std::vector<std::string> toStrings(const ResultKey& key);
