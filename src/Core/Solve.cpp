#include "Solve.hpp"
#include <cmath>
#include <iostream>

double solve(const Function& f, double x, double y)
{
    auto fx = f(x);
    auto fy = f(y);
    auto e = std::fabs(fy-fx);
    for (auto i=0; i<1000; ++i) {
        assert(x<=y);
        assert(fx*fy<=0.0);
        const auto z = (x+y)/2.0;
        const auto fz = f(z);
        if (fx*fz<=0.0) {
            y=z;
            fy=fz;
        } else {
            assert(fy*fz<=0.0);
            x=z;
            fx=fz;
        }
        const auto e_ = std::fabs(fy-fx);
        if (e_<e)
            e=e_;
        else
            return z;
    }
    assert(false && "too many iterations");
}