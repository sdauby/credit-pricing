#include "Models/S3/Utilities.hpp"

std::vector<int> indices(const std::vector<Date>& t, const std::vector<Date>& T)
{
    std::vector<int> v;
    v.reserve(t.size());
    
    const auto N = T.size() - 1;
    
    auto k = 0; // index in T
    for (const auto& x : t) {
        while (k<=N && T[k]<x)
            ++k;
        assert(k<=N && T[k]==x && "t must be included in T");
        v.push_back(k);
    }
    
    return v;
}

void includeT0(std::vector<Date>& t)
{
    using namespace std::chrono;
    constexpr auto T0 = Date{0y};
    if (t.empty() || t.front() != T0)
        t.insert(t.begin(),T0);
}
