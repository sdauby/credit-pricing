#pragma once

#include "Mutation.hpp"
#include "Container/InterestRateCurveId.hpp"

class IRCurveMutation : public Mutation {
public: 
    IRCurveMutation(const InterestRateCurveId& id, double rateShift);
    std::unique_ptr<Container> apply(const Container& container) const override;
private:
    InterestRateCurveId id_;
    double rateShift_ = {};
};
