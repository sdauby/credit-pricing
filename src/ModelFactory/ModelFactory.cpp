#include "ModelFactory.hpp"
#include "Models/Model.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"
#include "Container/InterestRateCurveId.hpp"
#include "Container/HazardRateCurveId.hpp"
#include "Container/S3ModelId.hpp"
#include "Core/Data.hpp"

#include <chrono>
#include "Instruments/IRSwap.hpp"
#include "Models/InterestRateCurve/IRCurveCalibration.hpp"
#include "Models/InterestRateCurve/IRSwapAnalytics.hpp"
#include <cmath>

namespace {

    template<typename T>
    constexpr bool always_false_v = false;

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    const Model* get(const Container& modelContainer, const ModelId& modelId)
    {
        return std::visit(overloaded {
            [&modelContainer](const InterestRateCurveId& irCurveId) -> const Model* {
                return modelContainer.get(irCurveId);
            },

            [&modelContainer](const HazardRateCurveId& hrCurveId) -> const Model* {
                return modelContainer.get(hrCurveId);
            },

            [&modelContainer](const S3ModelId& s3ModelId) -> const Model* {
                return modelContainer.get(s3ModelId);
            }
        }
        , modelId);
    }

    void set(Container& modelContainer, const ModelId& modelId, std::unique_ptr<Model>&& model)
    {
        std::visit(
            [&modelContainer, model = std::move(model)](auto&& modelId) mutable {
                using T = std::decay_t<decltype(modelId)>;
                if constexpr (std::is_same_v<T, InterestRateCurveId>) {
                    auto* model_ = model.release();
                    auto* irCurve = dynamic_cast<InterestRateCurve*>(model_);
                    auto irCurve_ = std::unique_ptr<InterestRateCurve>(irCurve);
                    modelContainer.set(modelId, std::move(irCurve_));
                } else if constexpr (std::is_same_v<T, HazardRateCurveId>) {
                    auto* model_ = model.release();
                    auto* hrCurve = dynamic_cast<HazardRateCurve*>(model_);
                    auto hrCurve_ = std::unique_ptr<HazardRateCurve>(hrCurve);
                    modelContainer.set(modelId, std::move(hrCurve_));
                } else if constexpr (std::is_same_v<T, S3ModelId>) {
                    auto* model_ = model.release();
                    auto* s3Model = dynamic_cast<S3Model*>(model_);
                    auto s3Model_ = std::unique_ptr<S3Model>(s3Model);
                    modelContainer.set(modelId, std::move(s3Model_));
                } else {
                    static_assert(always_false_v<T>, "non-exhaustive visitor");
                }
            }            
            , modelId);
    }
}


void populate(Container& modelContainer,
              const ModelId& modelId,
              const ModelFactory& modelFactory)
{
    const auto& precedents = modelFactory.precedents(modelId);
    for (const auto& precedent: precedents) {
        if (!get(modelContainer,precedent))
            populate(modelContainer,precedent,modelFactory);
    }
    auto model = modelFactory.make(modelId,modelContainer);
    set(modelContainer,modelId,std::move(model));
}

std::vector<ModelId> ModelFactory::precedents(const ModelId& modelId) const
{
    return std::visit(overloaded {
            [](const S3ModelId& s3ModelId) -> std::vector<ModelId> {
                const auto ccy = s3ModelId.ccy;
                const auto& issuer = s3ModelId.issuer;
                const auto& T = s3ModelId.tenorStructure;
                
                const auto irCurveId = InterestRateCurveId{ccy};
                const auto hrCurveId = HazardRateCurveId{issuer,ccy};

                return { irCurveId, hrCurveId };
            },
            [](const InterestRateCurveId& irCurveId) -> std::vector<ModelId> {
                return {};
            },
            [](const HazardRateCurveId& hrCurveId) -> std::vector<ModelId> {
                return {};
            }
        },
        modelId);
}


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

namespace {

    template<typename ObjectIdT>
    std::unique_ptr<Model> make_(const ObjectIdT& id, const Container& modelContainer);

    template<>
    std::unique_ptr<Model>
    make_<InterestRateCurveId>(const InterestRateCurveId& irCurveId, const Container& modelContainer)
    {
        auto swapRates = { 0.02, 0.02, 0.02 };
        return makeIRCurve(swapRates);
    } 

    template<>
    std::unique_ptr<Model>
    make_<HazardRateCurveId>(const HazardRateCurveId& hrCurveId, const Container& modelContainer)
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
    std::unique_ptr<Model>
    make_<S3ModelId>(const S3ModelId& s3ModelId, const Container& modelContainer)
    {
        const auto ccy = s3ModelId.ccy;
        const auto& issuer = s3ModelId.issuer;
        const auto& T = s3ModelId.tenorStructure;
        
        const auto irCurveId = InterestRateCurveId{ccy};
        const auto* irCurve = modelContainer.get(irCurveId);
        assert(irCurve);

        const auto hrCurveId = HazardRateCurveId{issuer,ccy};
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
}

std::unique_ptr<Model> ModelFactory::make(const ModelId& modelId, const Container& modelContainer) const
{
    return std::visit(
        [&modelContainer] (auto&& id) {
            return make_(id,modelContainer);
        },
        modelId);
}
