#pragma once
#include "Core/Issuer.hpp"
#include "Core/Currency.hpp"

namespace Data {
    namespace Issuers {
        extern const Issuer BNP;
        extern const Issuer JPM;
        extern const Issuer C;
    }

    double interestRate(Currency ccy);
    double hazardRate(const Issuer& issuer, Currency ccy);
    double recoveryRate(const Issuer& issuer, Currency ccy);
}