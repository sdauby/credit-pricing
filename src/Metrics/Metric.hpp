#pragma once

#include "Container/Container.hpp"
#include "Container/ContainerDag.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Result.hpp"
#include "Pricers/Pricer.hpp"
#include "Factory/Factory.hpp"
#include <set>

#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"
#include "Pricers/Pricer.hpp"
#include "Instruments/Instrument.hpp"

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
                                                  const Container& container) const = 0;
};

class Mutation {
public:
    virtual ~Mutation() = default;
    Mutation() = default;
    Mutation(const Mutation&) = delete;
    Mutation& operator=(const Mutation&) = delete;

    // The output modelContainer has a non-owning reference to the input modelContainer: the
    // caller must ensure that the lifetime of the input modelContainer is greater than the
    // lifetime of the output modelContainer.
    virtual std::unique_ptr<Container> apply(const Container& container) const = 0;
};

using MutationPtr = std::unique_ptr<Mutation>;

class NullMutation : public Mutation {
public:
    NullMutation() = default;

    std::unique_ptr<Container> apply(const Container& container) const override 
    {
        return std::make_unique<Container>(container);
    }
};
    
class IRCurveMutation : public Mutation {
public: 
    IRCurveMutation(const InterestRateCurveId& id, double rateShift) :
        id_(id),
        rateShift_(rateShift)
    {}

    std::unique_ptr<Container> apply(const Container& container) const override 
    {
        const auto* irCurve = container.get(id_);
        assert(irCurve);
        auto mutatedCurve = irCurve->applyRateShift(rateShift_);
        auto overlay = std::make_unique<Container>(container);
        overlay->set(id_,std::move(mutatedCurve));
        return overlay;
    }
private:
    InterestRateCurveId id_;
    double rateShift_ = {};
};

class UpdateMutation : public Mutation {
public:
    UpdateMutation(const std::vector<VariantId>& dirtyIds,
                   const ContainerDag& updateDag,
                   const Factory& factory) :
        dirtyIds_(dirtyIds),
        updateDag_(updateDag),
        factory_(factory)
    {}

    std::unique_ptr<Container> apply(const Container& container) const override 
    {
        auto overlay = std::make_unique<Container>(container);
        
        auto dirtyIds = std::set<VariantId>(dirtyIds_.cbegin(),dirtyIds_.cend());
        auto isDirty = [&dirtyIds] (const auto& id) { return dirtyIds.contains(id); };
        
        auto currentLayer = std::set<VariantId>();
        auto nextLayer = std::set<VariantId>(updateDag_.roots.cbegin(),updateDag_.roots.cend());

        while (!nextLayer.empty()) {
            currentLayer.swap(nextLayer);
            nextLayer.clear();
            for (const auto& id : currentLayer) {
                if (const auto i = updateDag_.precedents.find(id); i != updateDag_.precedents.end()) {
                    const auto& precedents = i->second;
                if (std::any_of(precedents.cbegin(),precedents.cend(),isDirty)) {
                    std::visit( [&] (const auto& id) { overlay->set( id, factory_.make(id,*overlay) ); }, id );
                    dirtyIds.insert(id);
                }
                }
                if (const auto i = updateDag_.dependents.find(id); i != updateDag_.dependents.end()) {
                    const auto& dependents = i->second;
                nextLayer.insert(dependents.cbegin(),dependents.cend());
            }
        }
        }
        return overlay;
    }

private:
    const std::vector<VariantId>& dirtyIds_;
    const ContainerDag& updateDag_;
    const Factory& factory_;
};

