#include "Core/Data.hpp"

namespace Data {

    namespace Issuers {
        using namespace std::string_literals;
        const Issuer BNP{"BNP"s};
        const Issuer JPM{"JPM"s};
        const Issuer C{"C"s};
    }

    double recoveryRate(const Issuer& issuer, Currency ccy) 
    { 
        return 0.5; 
    }

}