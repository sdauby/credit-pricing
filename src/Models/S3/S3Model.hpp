#pragma once
#include "Models/Model.hpp"
#include <vector>
#include "Core/Date.hpp"

class S3Model : public Model { // Schoenbucher chapter 3
public:
    S3Model(std::vector<Date>&& T, // T[k] <-> T_k, k = 0 .. K
            std::vector<double>&& F, // F[k] <-> F(T_0, T_k, T_{k+1}), k = 0 .. K-1
            std::vector<double>&& H, // H[k] <-> H(T_0, T_k, T_{k+1}), k = 0 .. K-1
            double pi); // pi

    const std::vector<Date>& T() const;
    double delta(int k) const; // delta_k
    double B(int k) const; // B(T_0,T_k)
    double Bbar(int k) const; // Bbar(T_0,T_k)
    double e(int k) const; // e(T_0,T_k,T_{k+1})
    double pi() const; // pi
    double F(int k1, int k2) const; // F(T_0, T_{k1}, T_{k2})

private:
    std::vector<Date> T_;
    std::vector<double> F_;
    std::vector<double> H_;
    double pi_ = 0.0;
};
