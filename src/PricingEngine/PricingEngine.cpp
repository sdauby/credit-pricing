#include "PricingEngine.hpp"
#include <iomanip>
#include <iostream>
#include "Instruments/Instrument.hpp"
#include "Instruments/Portfolio.hpp"
#include "ModelContainer/ModelContainer.hpp"
#include "Pricers/Pricer.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "PricingConfiguration.hpp"
#include "ModelFactory.hpp"

namespace {

    std::vector<std::unique_ptr<Pricer>> 
    makePricers(const InstrumentMap& instruments, const PricingConfiguration& config)
    {
        std::vector<InstrumentId> pricedWithIR;
        std::vector<InstrumentId> pricedWithS3;
        for (const auto& [id,instrument] : instruments) {
            if (const auto pricerKind = config.pricerKind(*instrument)) {
                switch (*pricerKind) {
                    case PricerKind::IR: pricedWithIR.emplace_back(id); break;
                    case PricerKind::S3: pricedWithS3.emplace_back(id); break;
                }
            }
        }

        std::vector<std::unique_ptr<Pricer>> pricers(2);
        pricers[0].reset(new IRPricer{instruments, std::move(pricedWithIR)});
        pricers[1].reset(new S3Pricer{instruments, std::move(pricedWithS3)});
        return pricers;
    }

}

namespace PricingEngine {

    PvResult price(const InstrumentMap& instruments, const PricingConfiguration& config)
    {
        const auto pricers = makePricers(instruments,config);

        ModelContainer modelContainer;
        PvResult instrumentPvs;
        for (const auto& pricer : pricers) {
            for (auto&& modelId : pricer->requiredModels())
                ModelFactory::populate(modelContainer,modelId);
            instrumentPvs.merge(pricer->pvs(modelContainer));
        }
        return instrumentPvs;
    }

}
