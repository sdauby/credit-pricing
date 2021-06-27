#include "PricingEngine.hpp"
#include <iomanip>
#include <iostream>
#include <set>
#include "Instruments/Instrument.hpp"
#include "Metrics/PVImpl.hpp"
#include "Metrics/IRDeltaImpl.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"
#include "Container/Container.hpp"
#include "Container/PricerId.hpp"
#include "Factory/Factory.hpp"
#include "Factory/PricerSubFactory.hpp"
#include "Factory/InstrumentSubFactory.hpp"
#include "Factory/HazardRateCurveSubFactory.hpp"
#include "Factory/InterestRateCurveSubFactory.hpp"
#include "Factory/S3ModelSubFactory.hpp"
#include "Pricers/Pricer.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "Pricers/PricingConfiguration.hpp"

namespace {
    template<typename IdT>
    void populate (Container& container, 
                   std::map<VariantId,std::vector<VariantId>>& allPrecedents,
                   const IdT& id, 
                   const Factory& factory) {
        auto precedents = factory.getPrecedents(id,container);
        for (const auto& precedent : precedents) {
            std::visit(
                [&container,&allPrecedents,&factory] (const auto& prec) {
                    if (!container.get(prec))
                        populate(container,allPrecedents,prec,factory);
                    assert(container.get(prec));
                },
                precedent);
        }
        container.set(id,factory.make(id,container));
        allPrecedents[id] = std::move(precedents);
    };

    Container initializeContainer(const Container& instruments,
                        const PricingConfiguration& config,
                                  const Factory& factory,
                                  ContainerDag& containerDag)
    {
        Container container(instruments);

        std::vector<PricerId> pricerIds{ PricerId{PricerKind::IR}, PricerId{PricerKind::S3} };
        for (const auto& id : pricerIds)
            populate(container,containerDag.precedents,id,factory);

        for (const auto& [id,precedentIds] : containerDag.precedents) {
            if (precedentIds.empty())
                containerDag.roots.push_back(id);
            for (const auto& p : precedentIds)
                containerDag.dependents[p].push_back(id);
        }
        
        return container;
    }

}

namespace PricingEngine {

    std::map<InstrumentId,Result> run(const Container& instruments, 
                                      const PricingConfiguration& config,
                                      const std::vector<Metric>& metrics)
    {
        Factory factory;
        factory.setSubFactory<PricerId           >(std::make_unique<PricerSubFactory>(config));
        factory.setSubFactory<InstrumentId       >(std::make_unique<InstrumentSubFactory>());
        factory.setSubFactory<HazardRateCurveId  >(std::make_unique<HazardRateCurveSubFactory>());
        factory.setSubFactory<InterestRateCurveId>(std::make_unique<InterestRateCurveSubFactory>());
        factory.setSubFactory<S3ModelId          >(std::make_unique<S3ModelSubFactory>());

        auto initializeDag = ContainerDag();
        const auto baseState = initializeContainer(instruments,config,factory,initializeDag);

        const auto makeMetricImpl = [&initializeDag,&factory] (Metric metric) -> std::unique_ptr<MetricImpl> {
            switch(metric) {
                case Metric::PV: return std::make_unique<PVImpl>();
                case Metric::IRDelta: return std::make_unique<IRDeltaImpl>(initializeDag,factory);
            }
        };

        std::map<InstrumentId,Result> results;
        for (const auto& pricerId : baseState.ids<PricerId>()) {
            for (const auto& metric : metrics) {
                const auto metricImpl = makeMetricImpl(metric);
                auto results_ = metricImpl->compute(*baseState.get(pricerId),baseState);
                for (auto& [instrumentId,result] : results_)
                    results[instrumentId].merge(result);
            }
        }
        return results;
    }
}
