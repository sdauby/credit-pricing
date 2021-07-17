#include "PricingEngine/PricingEngine.hpp"

#include "Container/Container.hpp"
#include "Container/PricerId.hpp"
#include "Elaboration/Elaboration.hpp"
#include "Elaboration/PricerBuilder.hpp"
#include "Elaboration/InstrumentBuilder.hpp"
#include "Elaboration/HazardRateCurveBuilder.hpp"
#include "Elaboration/InterestRateCurveBuilder.hpp"
#include "Elaboration/S3ModelBuilder.hpp"
#include "Metrics/PV.hpp"
#include "Metrics/IRDelta.hpp"

#include <fstream>

namespace PricingEngine {

    using namespace Metrics;

    std::map<InstrumentId,Result> run(const std::vector<InstrumentId>& instruments,
                                      const InstrumentFactory& makeInstrument,
                                      const PricingConfiguration& config,
                                      const std::vector<MetricKind>& metrics)
    {
        BuilderGeneralFactory factory;
        factory.setFactory<PricerId>( 
                [&config] (const PricerId& id) { return std::make_unique<PricerBuilder>(id,config); } 
            );
        factory.setFactory<InstrumentId>(
                [&makeInstrument] (const InstrumentId& id) { 
                    return std::make_unique<InstrumentBuilder>(makeInstrument,id); 
                } 
            );
        factory.setFactory<InterestRateCurveId>(
                [] (const InterestRateCurveId& id) { return std::make_unique<InterestRateCurveBuilder>(id); } 
            );
        factory.setFactory<HazardRateCurveId>(
                [] (const HazardRateCurveId& id) { return std::make_unique<HazardRateCurveBuilder>(id); } 
            );
        factory.setFactory<S3ModelId>(
                [] (const S3ModelId& id) { return std::make_unique<S3ModelBuilder>(id); } 
            );

        std::ostream * os = nullptr;
        std::ofstream f;
        const auto dumpElaborationGraph = false;
        if (dumpElaborationGraph) {
            f.open("elaboration.dot");
            os = &f;
        }

        const auto pricerId = PricerId { PricerKind::General, instruments };
        const auto x = elaborateContainer( { pricerId }, factory, os );
        const auto& container = std::get<0>(x);
        const auto& requests = std::get<1>(x);

        const auto makeMetricImpl = [&] (MetricKind metric) -> std::unique_ptr<Metric> {
            switch (metric) {
                case MetricKind::PV: return std::make_unique<Metrics::PV>();
                case MetricKind::IRDelta: return std::make_unique<IRDelta>(makeAux(requests),factory);
                default: assert(!"Metric not supported");
            }
        };

        std::map<InstrumentId,Result> results;
        for (const auto& metric : metrics) {
            const auto metricImpl = makeMetricImpl(metric);
            auto results_ = metricImpl->compute(container);
            for (auto& [instrumentId,result] : results_)
                results[instrumentId].merge(result);
        }
        return results;
    }
}
