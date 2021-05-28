#include "Data.hpp"

namespace Data {

    namespace Issuers {
        using namespace std::string_literals;
        const Issuer BNP{"BNP"s};
        const Issuer JPM{"JPM"s};
        const Issuer C{"C"s};
    }

    double interestRate(Currency ccy) 
    { 
        switch (ccy) {
            using Ccy = Currency;
            case Ccy::EUR: return 0.02;
            case Ccy::GBP: return 0.03;
            case Ccy::JPY: return 0.0;
            case Ccy::USD: return 0.01;
        }
    }
        
    double hazardRate(const Issuer& issuer, Currency ccy) {
        using Ccy = Currency;
        using namespace Issuers;
        if (issuer == BNP && ccy == Ccy::EUR)
            return 0.01;
        if (issuer == JPM && ccy == Ccy::USD)
            return 0.02;
        if (issuer == C && ccy == Ccy::USD)
            return 0.03;
        assert(false && "Missing hazard rate data");
        return 0.0;
    }

    double recoveryRate(const Issuer& issuer, Currency ccy) 
    { 
        return 0.5; 
    }

}