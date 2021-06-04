#include "InterestRateCurve.hpp"
#include <cmath>

InterestRateCurve::InterestRateCurve(double rate) : 
    InterestRateCurve(std::vector<Date>{}, std::vector<double>{rate})
{}

InterestRateCurve::InterestRateCurve(std::vector<Date>&& T, std::vector<double>&& r) :
    T_{std::move(T)},
    r_{std::move(r)}
{
    assert(r_.size() == T_.size()+1 && "bad IR curve construction");
}

double InterestRateCurve::discountFactor(Date t, Date u) const
{
    assert(t<=u && "bad discount factor arguments");

    const auto N = T_.size();

    const auto locate = [&T = T_, N] (Date x) {
        const auto i = std::lower_bound(T.cbegin(),T.cend(),x) - T.cbegin();
        assert(i==N || x<=T[i]);
        assert(i==0 || T[i-1]<x);
        return i;
    };
    const auto m = locate(t);
    const auto n = locate(u);
    assert(m<=n);

    auto df = 1.0;
    for (auto i=m; i<=n; ++i) {
        Date a = (i==m ? t : T_[i-1]);
        Date b = (i==n ? u : T_[i]);
        df *= std::exp( -r_[i] * dayCountFactor(a,b) ); // consider pre-calc at node points for perf
    }
    return df;
}

double forwardRate(const InterestRateCurve& curve, Date t, Date u)
{
    return 1.0/dayCountFactor(t,u) * ( 1.0 / curve.discountFactor(t,u) - 1.0 );
}