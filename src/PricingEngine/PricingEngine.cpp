#include "PricingEngine.hpp"
#include <iomanip>
#include <iostream>
#include <set>
#include <stack>
#include "Portfolio.hpp"
#include "Instruments/Instrument.hpp"
#include "Metrics/PVImpl.hpp"
#include "Metrics/IRDeltaImpl.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"
#include "Container/Container.hpp"
#include "Container/PricerId.hpp"
#include "Container/ContainerDag.hpp"
#include "Pricers/Pricer.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "Pricers/PricingConfiguration.hpp"
#include "Elaboration/Elaboration.hpp"
#include "Elaboration/PricerElaborator.hpp"
#include "Elaboration/InterestRateCurveElaborator.hpp"
#include "Elaboration/HazardRateCurveElaborator.hpp"
#include "Elaboration/S3ModelElaborator.hpp"
#include "Elaboration/InstrumentElaborator.hpp"

namespace PricingEngine {

    std::map<InstrumentId,Result> run(const std::vector<InstrumentId>& instruments,
                                      const InstrumentFactory& makeInstrument,
                                      const PricingConfiguration& config,
                                      const std::vector<Metric>& metrics)
    {
        ElaboratorGeneralFactory factory;
        factory.setFactory<PricerId>( 
                [&config] (const PricerId& id) { return std::make_unique<PricerElaborator>(id,config); } 
            );
        factory.setFactory<InstrumentId>(
                [&makeInstrument] (const InstrumentId& id) { 
                    return std::make_unique<InstrumentElaborator>(makeInstrument,id); 
                } 
            );
        factory.setFactory<InterestRateCurveId>(
                [] (const InterestRateCurveId& id) { return std::make_unique<InterestRateCurveElaborator>(id); } 
            );
        factory.setFactory<HazardRateCurveId>(
                [] (const HazardRateCurveId& id) { return std::make_unique<HazardRateCurveElaborator>(id); } 
            );
        factory.setFactory<S3ModelId>(
                [] (const S3ModelId& id) { return std::make_unique<S3ModelElaborator>(id); } 
            );

        const auto pricerId = PricerId { PricerKind::General, instruments };
        auto [container,dag] = elaborateContainer( { pricerId }, factory); 

        const auto makeMetricImpl = [] (Metric metric) {
            assert(metric == Metric::PV);
            return std::make_unique<PVImpl>();
        };

        std::map<InstrumentId,Result> results;
        for (const auto& metric : metrics) {
            const auto metricImpl = makeMetricImpl(metric);
            auto results_ = metricImpl->compute(*container.get(pricerId),container);
            for (auto& [instrumentId,result] : results_)
                results[instrumentId].merge(result);
        }
        return results;
    }
}
