#include "Factory.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Core/Data.hpp"

template<>
std::vector<VariantId> Factory::getPrecedents(const HazardRateCurveId&, const Container&) const
{
    return {};
}

template<>
std::unique_ptr<HazardRateCurve> Factory::make(const HazardRateCurveId& hrCurveId, const Container& container) const
{
    double lambda = [&issuer = hrCurveId.issuer, ccy = hrCurveId.ccy]() {
        using Ccy = Currency;
        using namespace Data::Issuers;
        if (issuer == BNP && ccy == Ccy::EUR)
            return 0.01;
        if (issuer == JPM && ccy == Ccy::USD)
            return 0.02;
        if (issuer == C && ccy == Ccy::USD)
            return 0.03;
        assert(false && "Missing hazard rate data");
        return 0.0;
    }();
    return std::make_unique<HazardRateCurve>(lambda);
}
