#include "IRCurveCalibration.hpp"
#include "Core/Solve.hpp"
#include "Pricers/IR/IRSwapAnalytics.hpp"

IRCurveCalibration::IRCurveCalibration(InstrumentVector&& instruments) :
    instruments_(std::move(instruments))
{
    const auto N = instruments_.size();
    assert(N>0);
    T_.reserve(N-1);
    for (auto i=0; i<N-1; ++i)
        T_.push_back(instruments_[i]->data().t.back());
    assert(is_sorted(T_.begin(),T_.end()));
}

std::unique_ptr<InterestRateCurve> IRCurveCalibration::calibrate(const PriceVector& prices) const
{
    const auto N = instruments_.size();
    assert(prices.size() == N);

    auto curve = make_unique<InterestRateCurve>(std::vector<Date>(T_),std::vector<double>(N,0.0));

    for (auto i=0; i<N; i++) {
        const auto f = [&curve = *curve, i, &instrument = *instruments_[i], price = prices[i]] (double r) {
            curve.r_[i] = r;
            return swapRate(instrument,curve) - price;
        };
        auto r = solve(f,-1.0,1.0);
        curve->r_[i] = r;
    }

    return curve;
}

const IRCurveCalibration::InstrumentVector& IRCurveCalibration::instruments() const
{
    return instruments_;
}
