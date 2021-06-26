#include "HazardRateCurveSubFactory.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Core/Data.hpp"

std::vector<VariantId> 
HazardRateCurveSubFactory::getPrecedents(const HazardRateCurveId& hrCurveId, const Container& container) const
{
    return {};
}

std::unique_ptr<HazardRateCurve> 
HazardRateCurveSubFactory::make(const HazardRateCurveId& hrCurveId, const Container& container) const
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
