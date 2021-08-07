#include "Models/InterestRateCurve/IRSwapAnalytics.hpp"

#include "Core/Date.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"

double annuity(const IRSwap& swap, const InterestRateCurve& curve)
{
    const auto df = [&curve](Date t) {
        using namespace std::chrono;
        constexpr auto T0 = 0y;
        return curve.discountFactor(T0,t);
    };

    const auto& t = swap.data().t;
    const auto N = t.size();

    auto annuity = 0.0;
    for (auto i=1u; i<N; ++i)
        annuity += dayCountFactor(t[i-1],t[i]) * df(t[i]);
    
    return annuity;
}

double swapRate(const IRSwap& swap, const InterestRateCurve& curve)
{
    const auto df = [&curve](Date t) {
        using namespace std::chrono;
        constexpr auto T0 = 0y;
        return curve.discountFactor(T0,t);
    };

    const auto& t = swap.data().t;

    const auto floatingLeg = df(t.front()) - df(t.back());
    const auto annuity = ::annuity(swap,curve);

    return floatingLeg / annuity;
}
