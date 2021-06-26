#include "S3ModelSubFactory.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/S3/S3Model.hpp"
#include "Core/Data.hpp"

std::vector<VariantId> S3ModelSubFactory::getPrecedents(const S3ModelId& s3ModelId, const Container& container) const
{
    const auto ccy = s3ModelId.ccy;
    const auto& issuer = s3ModelId.issuer;
                
    const auto irCurveId = InterestRateCurveId{ccy};
    const auto hrCurveId = HazardRateCurveId{issuer,ccy};

    return { irCurveId, hrCurveId };
}

std::unique_ptr<S3Model> S3ModelSubFactory::make(const S3ModelId& s3ModelId, const Container& container) const
{
    const auto ccy = s3ModelId.ccy;
    const auto& issuer = s3ModelId.issuer;
    const auto& T = s3ModelId.tenorStructure;
        
    const auto irCurveId = InterestRateCurveId{ccy};
    const auto* irCurve = container.get(irCurveId);
    assert(irCurve);

    const auto hrCurveId = HazardRateCurveId{issuer,ccy};
    const auto* hrCurve = container.get(hrCurveId);
    assert(hrCurve);

    const auto recoveryRate = Data::recoveryRate(issuer,ccy);
    
    const auto K = T.size()-1;
    std::vector<double> F(K);
    std::vector<double> H(K);
    for (auto k=0; k<K; ++k) {
        F[k] = forwardRate(*irCurve,T[k],T[k+1]);
        H[k] = hazardRate(*hrCurve,T[k],T[k+1]);
    }
    return std::make_unique<S3Model>(std::vector<Date>(T),std::move(F),std::move(H),recoveryRate); 
}
