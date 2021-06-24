#pragma once
#include "Core/Issuer.hpp"
#include "Core/Currency.hpp"
#include "Core/Date.hpp"
#include <vector>
#include <tuple>

class S3Model;

struct S3ModelId {
    using ObjectType = S3Model;
    Issuer issuer;
    Currency ccy = {};
    std::vector<Date> tenorStructure;
};

inline bool operator<(const S3ModelId& x, 
                      const S3ModelId& y)
{
    return std::tuple{x.issuer, x.ccy, x.tenorStructure, } 
        <  std::tuple{y.issuer, y.ccy, y.tenorStructure, };
}
