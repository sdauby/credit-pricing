#pragma once

#include "ModelContainer/ModelContainer.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Result.hpp"
#include "Pricers/Pricer.hpp"

enum class Metric {
    PV,
    IRDelta,
};


class MetricImpl {
public:
    virtual ~MetricImpl() = default;
    MetricImpl() = default;
    MetricImpl(const MetricImpl&) = delete;
    MetricImpl& operator=(const MetricImpl&) = delete;
    
    virtual std::map<InstrumentId,Result> compute(const Pricer& pricer,
                                                  const ModelContainer& modelContainer) const = 0;
};

std::unique_ptr<MetricImpl> makeMetricImpl(Metric metric);

class Mutation {
public:
    virtual ~Mutation() = default;
    Mutation() = default;
    Mutation(const Mutation&) = delete;
    Mutation& operator=(const Mutation&) = delete;

    // The output modelContainer has a non-owning reference to the input modelContainer: the
    // caller must ensure that the lifetime of the input modelContainer is greater than the
    // lifetime of the output modelContainer.
    virtual std::unique_ptr<ModelContainer> apply(const ModelContainer& modelContainer) const = 0;
};

using MutationPtr = std::unique_ptr<Mutation>;

class NullMutation : public Mutation {
public:
    NullMutation() = default;

    std::unique_ptr<ModelContainer> apply(const ModelContainer& modelContainer) const override 
    {
        return std::make_unique<ModelContainer>(&modelContainer);
    }
};
    
class IRCurveMutation : public Mutation {
public: 
    IRCurveMutation(const InterestRateCurveId& id, double rateShift) :
        id_(id),
        rateShift_(rateShift)
    {}

    std::unique_ptr<ModelContainer> apply(const ModelContainer& modelContainer) const override 
    {
        const auto* irCurve = modelContainer.get(id_);
        assert(irCurve);
        auto mutatedCurve = irCurve->applyRateShift(rateShift_);
        auto overlay = std::make_unique<ModelContainer>(&modelContainer);
        overlay->set(id_,std::move(mutatedCurve));
        return overlay;
    }
private:
    InterestRateCurveId id_;
    double rateShift_ = {};
};

