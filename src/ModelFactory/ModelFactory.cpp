#include "ModelFactory.hpp"
#include "Core/Data.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"

#include <chrono>
#include "Instruments/IRSwap.hpp"
#include "Models/InterestRateCurve/IRCurveCalibration.hpp"
#include "Models/InterestRateCurve/IRSwapAnalytics.hpp"
#include <cmath>

namespace {
    // annual period dates, unspecified currency, zero fixed rate
    std::unique_ptr<IRSwap> makeIRSwap(int nPeriods)
    {
        const auto ccy = Currency{};
        const auto rate = 0.0;

        using namespace std::chrono;

        std::vector<Date> ts(nPeriods+1);
        for (auto i=0; i<=nPeriods; ++i)
            ts[i]=year(i);

        return make<IRSwap>(ccy,std::move(ts),rate);
    }

    std::vector<std::unique_ptr<IRSwap>> makeIRSwapStrip(int N)
    {
        std::vector<std::unique_ptr<IRSwap>> swaps(N);
        for (auto i=1; i<=N; ++i)
            swaps[i-1] = makeIRSwap(i);
        return swaps;
    }

    // Given the rates s1,s2...sn of the 1Y,2Y...NY swaps, return the corresponding IR curve
    std::unique_ptr<InterestRateCurve> makeIRCurve(const std::vector<double>&& swapRates)
    {
        const auto N = swapRates.size();
        auto swaps = makeIRSwapStrip(N);
        IRCurveCalibration calibration(std::move(swaps));
        
        auto curve = calibration.calibrate(swapRates);
        
        const auto& instruments = calibration.instruments();
        for (auto i=0; i<N; ++i)
            assert(std::fabs( swapRate(*instruments[i],*curve) - swapRates[i] ) < 1e-8);

        return curve;
    }
}


namespace ModelFactory {

    void populate(ModelContainer& modelContainer, const ModelId& modelId);

    template<typename ModelIdT>
    ModelPtr<ModelIdT> make(const ModelIdT& id, ModelContainer& modelContainer);

    template<>
    ModelPtr<InterestRateCurveId> 
    make<InterestRateCurveId>(const InterestRateCurveId& irCurveId, ModelContainer& modelContainer)
    {
        auto swapRates = { 0.02, 0.02, 0.02 };
        return makeIRCurve(swapRates);
    } 

    template<>
    ModelPtr<HazardRateCurveId> 
    make<HazardRateCurveId>(const HazardRateCurveId& hrCurveId, ModelContainer& modelContainer)
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

    template<>
    ModelPtr<S3ModelId> 
    make<S3ModelId>(const S3ModelId& s3ModelId, ModelContainer& modelContainer)
    {
        const auto ccy = s3ModelId.ccy;
        const auto& issuer = s3ModelId.issuer;
        const auto& T = s3ModelId.tenorStructure;
        
        const auto irCurveId = InterestRateCurveId{ccy};
        populate(modelContainer,irCurveId);
        const auto* irCurve = modelContainer.get(irCurveId);
        assert(irCurve);

        const auto hrCurveId = HazardRateCurveId{issuer,ccy};
        populate(modelContainer,hrCurveId);
        const auto* hrCurve = modelContainer.get(hrCurveId);
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

    // Cf Stroustrup, A Tour of C++, 13.5.1
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
    
    void populate(ModelContainer& modelContainer, const ModelId& modelId)
    {
        std::visit(overloaded {
            [&modelContainer](const InterestRateCurveId& irCurveId) {
                const auto* ptr = modelContainer.get(irCurveId);
                if (!ptr)
                    modelContainer.set(irCurveId,make(irCurveId,modelContainer));
            },

            [&modelContainer](const HazardRateCurveId& hrCurveId) {
                const auto* ptr = modelContainer.get(hrCurveId);
                if (!ptr)
                    modelContainer.set(hrCurveId,make(hrCurveId,modelContainer));
            },

            [&modelContainer](const S3ModelId& s3ModelId) {
                const auto* ptr = modelContainer.get(s3ModelId);
                if (!ptr)
                    modelContainer.set(s3ModelId,make(s3ModelId,modelContainer));
            },
        },modelId);
    }

}
