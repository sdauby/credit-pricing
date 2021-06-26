#include "PricingEngine.hpp"
#include <iomanip>
#include <iostream>
#include <set>
#include "Instruments/Instrument.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"
#include "Container/Container.hpp"
#include "Container/PricerId.hpp"
#include "Factory/Factory.hpp"
#include "ModelFactory/ModelFactory.hpp"
#include "Pricers/Pricer.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "Pricers/PricingConfiguration.hpp"


namespace {

    template<typename IdT>
    void populate (Container& container, const IdT& id, const Factory& factory) {
        const auto precedents = factory.getPrecedents(id,container);
        for (const auto& precedent : precedents) {
            std::visit(
                [&container,&factory] (const auto& prec) {
                    if (!container.get(prec))
                        populate(container,prec,factory);
                    assert(container.get(prec));
                },
                precedent);
        }
        container.set(id,factory.make(id,container));
    };

    Container initializeContainer(const Container& instruments,
                             const PricingConfiguration& config)
    {
        Container container(instruments);
        const Factory factory(config);

        std::vector<PricerId> pricerIds{ PricerId{PricerKind::IR}, PricerId{PricerKind::S3} };
        for (const auto& id : pricerIds)
            populate(container,id,factory);
        
        return container;
    }

}

namespace PricingEngine {

    std::map<InstrumentId,Result> run(const Container& instruments, 
                                      const PricingConfiguration& config,
                                      const std::vector<Metric>& metrics)
    {
        Container baseState = initializeContainer(instruments,config);

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
