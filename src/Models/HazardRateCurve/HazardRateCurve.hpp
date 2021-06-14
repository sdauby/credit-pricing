#pragma once
#include "Models/Model.hpp"
#include "Core/Date.hpp"

class HazardRateCurve : public Model {
public:
    explicit HazardRateCurve(double lambda);

    double survivalProbability(Date t1, Date t2) const;

private:
    double lambda_ = 0.0;
};

double hazardRate(const HazardRateCurve& curve, Date t1, Date t2);
