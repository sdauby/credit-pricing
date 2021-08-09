#include "Pricers/S3/FixedCouponBondPricing.hpp"

#include "Container/Ids/S3ModelId.hpp"
#include "Models/S3/S3Model.hpp"
#include "Models/S3/Utilities.hpp"

S3ModelId modelId(const FixedCouponBond& bond) 
{
    const auto& data = bond.data();
    auto t = data.t;
    includeT0(t);
    return {
        .issuer = data.issuer,
        .ccy = data.ccy,
        .tenorStructure = std::move(t),
    };
}

double pv(const FixedCouponBond& bond, const S3Model& model)
{
    const auto& T = model.T();
    const auto& data = bond.data();
    const auto N = data.t.size()-1;
    const auto k = indices(data.t,T);

    auto pv = 0.0;

    // coupons
    const auto c = data.c;
    for (auto n=1u; n<=N; ++n) {
        const auto d = dayCountFactor(T[k[n-1]],T[k[n]]); // delta'_{n-1}
        pv += c * d * model.Bbar(k[n]);
    }

    // principal
    pv += model.Bbar(k[N]);

    // recovery
    const auto pi = model.pi();
    for (auto k_=1; k_<=k[N]; ++k_) {
        pv += pi * model.e(k_-1);
    }

    return pv;
}
