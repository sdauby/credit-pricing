#include "S3Model.hpp"

S3Model::S3Model(std::vector<Date>&& T,
             std::vector<double>&& F,
             std::vector<double>&& H,
             double pi) :
             T_(std::move(T)),
             F_(std::move(F)),
             H_(std::move(H)),
             pi_(pi)
{
    const auto K = T_.size() - 1;
    assert(F_.size() == K && "F must have K elements");
    assert(H_.size() == K && "H must have K elements");
}

const std::vector<Date>& S3Model::T() const
{
    return T_;
}

double S3Model::delta(int k) const
{
    return dayCountFactor(T_[k],T_[k+1]);
}

double S3Model::B(int k) const
{
    auto x = 1.0;
    for (auto i=1; i<=k; ++i) {
        x *= (1.0 + delta(i-1) * F_[i-1]);
    }
    return 1.0/x;
}

double S3Model::Bbar(int k) const
{
    auto x = 1.0;
    for (auto i=1; i<=k; ++i) {
        const auto d = delta(i-1);
        x *= (1.0 + d * F_[i-1]) * (1.0 + d * H_[i-1]);
    }
    return 1.0/x;
}
    
double S3Model::e(int k) const {
    return delta(k) * H_[k] * Bbar(k+1); // Schoenbucher (3.20) has Bbar(k), incorrectly.
}
    
double S3Model::pi() const {
    return pi_;
}

double S3Model::F(int k1, int k2) const {
    auto b = 1.0;
    for (auto k=k1; k<k2; ++k) {
        b *= (1.0 + dayCountFactor(T_[k],T_[k+1]) * F_[k]);
    }
    return (b-1.0)/dayCountFactor(T_[k1],T_[k2]);
}
    
