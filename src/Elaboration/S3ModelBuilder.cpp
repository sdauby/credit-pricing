#include "Elaboration/S3ModelBuilder.hpp"

#include "Core/Data.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/S3/S3Model.hpp"

S3ModelBuilder::S3ModelBuilder(const S3ModelId& id) : id_(id) {}

std::vector<VariantId> S3ModelBuilder::getRequestBatch(const Container&)
{
    switch (state_) {
        case State::Initial:
            state_ = State::CurvesRequested;
            return { InterestRateCurveId{id_.ccy}, HazardRateCurveId{id_.issuer,id_.ccy} };
        case State::CurvesRequested:
            state_ = State::Final;
            return {};
        case State::Final:
            assert(!"unexpected state for getRequestBatch() call");
            return {};
    }
}

std::unique_ptr<S3Model> S3ModelBuilder::getObject(const Container& container)
{
    const auto ccy = id_.ccy;
    const auto& issuer = id_.issuer;
    const auto& T = id_.tenorStructure;
        
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
    for (auto k=0u; k<K; ++k) {
        F[k] = forwardRate(*irCurve,T[k],T[k+1]);
        H[k] = hazardRate(*hrCurve,T[k],T[k+1]);
    }
    return std::make_unique<S3Model>(std::vector<Date>(T),std::move(F),std::move(H),recoveryRate); 
}
