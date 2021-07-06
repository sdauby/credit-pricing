#include "HazardRateCurveBuilder.hpp"
#include "Core/Data.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"

HazardRateCurveBuilder::HazardRateCurveBuilder(const HazardRateCurveId& id) : id_(id) {}

std::vector<VariantId> HazardRateCurveBuilder::getRequestBatch(const Container&)
{
    return {};
}

std::unique_ptr<HazardRateCurve> HazardRateCurveBuilder::getObject(const Container& container)
{
    const double lambda = [&issuer = id_.issuer, ccy = id_.ccy]() {
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
