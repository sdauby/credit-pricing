#include "InterestRateCurveBuilder.hpp"
#include "Instruments/IRSwap.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
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

InterestRateCurveBuilder::InterestRateCurveBuilder(const InterestRateCurveId& id) : id_(id) {}

std::vector<VariantId> InterestRateCurveBuilder::getRequestBatch(const Container&)
{
    return {};
}

std::unique_ptr<InterestRateCurve> InterestRateCurveBuilder::getObject(const Container&)
{
    auto swapRates = { 0.02, 0.02, 0.02 };
    return makeIRCurve(swapRates);
}
