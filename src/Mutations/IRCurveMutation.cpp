#include "IRCurveMutation.hpp"
#include "Container/Container.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"

IRCurveMutation::IRCurveMutation(const InterestRateCurveId& id, double rateShift) :
    id_(id),
    rateShift_(rateShift)
{}

std::unique_ptr<Container> IRCurveMutation::apply(const Container& container) const
{
    const auto* irCurve = container.get(id_);
    assert(irCurve);
    auto mutatedCurve = irCurve->applyRateShift(rateShift_);
    auto overlay = std::make_unique<Container>(container);
    overlay->set(id_,std::move(mutatedCurve));
    return overlay;
}
