#include "CdsPricing.hpp"
#include "Models/S3/S3Model.hpp"
#include "Models/S3/Utilities.hpp"
#include "ModelContainer/S3ModelId.hpp"

S3ModelId modelId(const Cds& cds)
{
    const auto& data = cds.data();
    auto t = data.t;
    includeT0(t);
    return {
        .issuer = data.refEntity,
        .ccy = data.ccy,
        .tenorStructure = std::move(t),
    };
}

double pv(const Cds& cds, const S3Model& model)
{
    const auto& T = model.T();
    const auto& data = cds.data();
    const auto N = data.t.size()-1;
    const auto k = indices(data.t,T);

    auto pv = 0.0;

    // fee leg
    const auto s = data.s;
    for (auto n=1; n<=N; ++n) {
        const auto d = dayCountFactor(T[k[n-1]],T[k[n]]); // delta'_{n-1}
        pv -= s * d * model.Bbar(k[n]); 
    }

    // protection leg
    const auto pi = model.pi();
    for (auto k_=k[0]+1; k_<=k[N]; ++k_) { // Schoenbucher (3.33) starts from k[1] but I think it is incorrect.
        pv += (1.0-pi) * model.e(k_-1);
    }

    return pv;
}