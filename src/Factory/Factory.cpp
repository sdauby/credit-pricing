#include "Factory.hpp"
#include "Core/Data.hpp"
#include "Instruments/IRSwap.hpp"
#include "Pricers/PricerKind.hpp"
#include "Pricers/PricingConfiguration.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/InterestRateCurve/IRCurveCalibration.hpp"
#include "Models/InterestRateCurve/IRSwapAnalytics.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"
#include <cmath>


struct Factory::Impl {
    explicit Impl(const PricingConfiguration& config) : config(config) {}
    const PricingConfiguration& config;
};

Factory::~Factory() = default;
Factory::Factory(const PricingConfiguration& config) : pImpl(std::make_unique<Impl>(config)) {}

template<>
std::vector<VariantId> Factory::getPrecedents(const InstrumentId&, const Container&) const
{ 
    return {}; 
}

template<>
std::unique_ptr<Object<InstrumentId>> Factory::make(const InstrumentId&, const Container&) const
{
    return {};
}


template<> 
std::vector<VariantId> Factory::getPrecedents(const PricerId& pricerId, const Container& container) const
{
    std::vector<VariantId> precedents;
    std::vector<InstrumentId> instruments;
    for (const auto& instrumentId : container.ids<InstrumentId>()) {
        if ( pImpl->config.pricerKind( *container.get(instrumentId) ) == pricerId.kind ) {
            instruments.emplace_back(instrumentId);
            precedents.emplace_back(instrumentId);
        }
    }
    
    auto modelIds = [&] () {
        switch (pricerId.kind) {
            case PricerKind::IR: return IRPricer(container,instruments).requiredModels();
            case PricerKind::S3: return S3Pricer(container,instruments).requiredModels();
        }
    } ();
    std::move(modelIds.begin(),modelIds.end(),std::back_inserter(precedents));

    return precedents;
}

template<>
std::unique_ptr<Object<PricerId>> Factory::make(const PricerId& pricerId, const Container& container) const
{
    std::vector<InstrumentId> instruments;
    for (const auto& instrumentId : container.ids<InstrumentId>()) {
        if ( pImpl->config.pricerKind( *container.get(instrumentId) ) == pricerId.kind ) 
            instruments.emplace_back(instrumentId);
    }

    switch (pricerId.kind) {
        case PricerKind::IR: return std::make_unique<IRPricer>(container,instruments);
        case PricerKind::S3: return std::make_unique<S3Pricer>(container,instruments);
    }
}


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



template<>
std::vector<VariantId> Factory::getPrecedents(const InterestRateCurveId&, const Container&) const
{
    return {};
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

template<> 
std::unique_ptr<InterestRateCurve> Factory::make(const InterestRateCurveId& irCurveId, const Container& container) const
{
    auto swapRates = { 0.02, 0.02, 0.02 };
    return makeIRCurve(swapRates);
}



template<>
std::vector<VariantId> Factory::getPrecedents(const S3ModelId& s3ModelId, const Container&) const
{
    const auto ccy = s3ModelId.ccy;
    const auto& issuer = s3ModelId.issuer;
                
    const auto irCurveId = InterestRateCurveId{ccy};
    const auto hrCurveId = HazardRateCurveId{issuer,ccy};

    return { irCurveId, hrCurveId };
}

template<> 
std::unique_ptr<S3Model> Factory::make(const S3ModelId& s3ModelId, const Container& container) const
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
