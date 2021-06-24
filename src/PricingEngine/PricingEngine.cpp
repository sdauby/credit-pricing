#include "PricingEngine.hpp"
#include <iomanip>
#include <iostream>
#include <set>
#include "Instruments/Instrument.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Container/Container.hpp"
#include "Container/PricerId.hpp"
#include "ModelFactory/ModelFactory.hpp"
#include "Pricers/Pricer.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "PricingConfiguration.hpp"

namespace {

    Container makeBaseState(const Container& instruments,
                            const PricingConfiguration& config)
    {
        Container baseState(instruments);

        std::vector<InstrumentId> pricedWithIR;
        std::vector<InstrumentId> pricedWithS3;
        for (const auto& id : instruments.ids<InstrumentId>()) {
            switch ( config.pricerKind( *instruments.get(id) ) ) {
                case PricerKind::IR: pricedWithIR.emplace_back(id); break;
                case PricerKind::S3: pricedWithS3.emplace_back(id); break;
            }
        }
        auto irPricer = std::make_unique<IRPricer>(instruments, pricedWithIR);
        auto s3Pricer = std::make_unique<S3Pricer>(instruments, pricedWithS3);
        std::array<std::unique_ptr<Pricer>,2> pricers{std::move(irPricer),std::move(s3Pricer)};

        ModelFactory modelFactory;
        for (const auto& pricer : pricers) {
            for (const auto& modelId : pricer->requiredModels())
                populate(baseState,modelId,modelFactory);
        }

        for (size_t k=0, n=pricers.size(); k<n; ++k)
            baseState.set(PricerId{k},std::move(pricers[k]));

        return baseState;
    }
}

namespace PricingEngine {

    std::map<InstrumentId,Result> run(const Container& instruments, 
                                      const PricingConfiguration& config,
                                      const std::vector<Metric>& metrics)
    {
        Container baseState = makeBaseState(instruments,config);

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
