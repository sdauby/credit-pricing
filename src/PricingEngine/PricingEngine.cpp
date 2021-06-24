#include "PricingEngine.hpp"
#include <iomanip>
#include <iostream>
#include <set>
#include "Instruments/Instrument.hpp"
#include "Instruments/Portfolio.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Container/Container.hpp"
#include "ModelFactory/ModelFactory.hpp"
#include "Pricers/Pricer.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "PricingConfiguration.hpp"

namespace {

    std::vector<std::unique_ptr<Pricer>> 
    makePricers(const InstrumentMap& instruments, const PricingConfiguration& config)
    {
        std::vector<InstrumentId> pricedWithIR;
        std::vector<InstrumentId> pricedWithS3;
        for (const auto& [id,instrument] : instruments) {
            switch (config.pricerKind(*instrument)) {
                case PricerKind::IR: pricedWithIR.emplace_back(id); break;
                case PricerKind::S3: pricedWithS3.emplace_back(id); break;
            }
        }

        std::vector<std::unique_ptr<Pricer>> pricers(2);
        pricers[0].reset(new IRPricer{instruments, std::move(pricedWithIR)});
        pricers[1].reset(new S3Pricer{instruments, std::move(pricedWithS3)});
        return pricers;
    }

}

namespace PricingEngine {

    std::map<InstrumentId,Result> run(const InstrumentMap& instruments, 
                                      const PricingConfiguration& config,
                                      const std::vector<Metric>& metrics)
    {
        const auto pricers = makePricers(instruments,config);

        Container modelContainer;
        ModelFactory modelFactory;
        for (const auto& pricer : pricers) {
            for (const auto& modelId : pricer->requiredModels())
                populate(modelContainer,modelId,modelFactory);
        }

        std::map<InstrumentId,Result> results;
        for (const auto& metric : metrics) {
            const auto metricImpl = makeMetricImpl(metric);
            for (const auto& pricer : pricers) {
                auto results_ = metricImpl->compute(*pricer,modelContainer);
                for (auto& [instrumentId,result] : results_)
                    results[instrumentId].merge(result);
            }
        }
        return results;
    }
}
