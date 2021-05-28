#include "PricingEngine.hpp"
#include <iomanip>
#include <iostream>
#include <map>
#include "Core/Currency.hpp"
#include "Core/Data.hpp"
#include "Instruments/Instrument.hpp"
#include "Instruments/InstrumentKind.hpp"
#include "Instruments/Portfolio.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"
#include "ModelContainer/ModelContainer.hpp"
#include "Pricers/Pricer.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"
#include "PricingConfiguration.hpp"

namespace ModelFactory {

    void populate(ModelContainer& modelContainer, const ModelId& modelId);

    template<typename ModelIdT>
    ModelPtr<ModelIdT> make(const ModelIdT& id, ModelContainer& modelContainer);

    template<>
    ModelPtr<InterestRateCurveId> 
    make<InterestRateCurveId>(const InterestRateCurveId& irCurveId, ModelContainer& modelContainer)
    {
        return std::make_unique<InterestRateCurve>(Data::interestRate(irCurveId.ccy));
    } 

    template<>
    ModelPtr<HazardRateCurveId> 
    make<HazardRateCurveId>(const HazardRateCurveId& hrCurveId, ModelContainer& modelContainer)
    {
        return std::make_unique<HazardRateCurve>(Data::hazardRate(hrCurveId.issuer,hrCurveId.ccy));
    } 

    template<>
    ModelPtr<S3ModelId> 
    make<S3ModelId>(const S3ModelId& s3ModelId, ModelContainer& modelContainer)
    {
        const auto ccy = s3ModelId.ccy;
        const auto& issuer = s3ModelId.issuer;
        const auto& T = s3ModelId.tenorStructure;
        
        const auto irCurveId = InterestRateCurveId{ccy};
        populate(modelContainer,irCurveId);
        const auto* irCurve = modelContainer.get(irCurveId);
        assert(irCurve);

        const auto hrCurveId = HazardRateCurveId{issuer,ccy};
        populate(modelContainer,hrCurveId);
        const auto* hrCurve = modelContainer.get(hrCurveId);
        assert(hrCurve);

        const auto recoveryRate = Data::recoveryRate(issuer,ccy);
    
        const auto K = T.size()-1;
        std::vector<double> F(K);
        std::vector<double> H(K);
        for (auto k=0; k<K; ++k) {
            F[k] = forwardRate(*irCurve,T[k],T[k+1]);
            H[k] = hrCurve->rate(T[k],T[k+1]);
        }
        return std::make_unique<S3Model>(std::vector<Date>(T),std::move(F),std::move(H),recoveryRate);
    } 

    // Cf Stroustrup, A Tour of C++, 13.5.1
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
    
    void populate(ModelContainer& modelContainer, const ModelId& modelId)
    {
        std::visit(overloaded {
            [&modelContainer](const InterestRateCurveId& irCurveId) {
                const auto* ptr = modelContainer.get(irCurveId);
                if (!ptr)
                    modelContainer.set(irCurveId,make(irCurveId,modelContainer));
            },

            [&modelContainer](const HazardRateCurveId& hrCurveId) {
                const auto* ptr = modelContainer.get(hrCurveId);
                if (!ptr)
                    modelContainer.set(hrCurveId,make(hrCurveId,modelContainer));
            },

            [&modelContainer](const S3ModelId& s3ModelId) {
                const auto* ptr = modelContainer.get(s3ModelId);
                if (!ptr)
                    modelContainer.set(s3ModelId,make(s3ModelId,modelContainer));
            },
        },modelId);
    }

}

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

    void price(const Portfolio& p, const PricingConfiguration& config)
    {
        const auto pricers = makePricers(p.instruments,config);

        ModelContainer modelContainer;
        PvResult instrumentPvs;
        for (const auto& pricer : pricers) {
            for (auto&& modelId : pricer->requiredModels())
                ModelFactory::populate(modelContainer,modelId);
            instrumentPvs.merge(pricer->pvs(modelContainer));
        }

        const auto printRow = [](char fill,
                                 auto positionId, 
                                 auto instrumentId,
                                 auto instrumentKind,
                                 auto ccy,
                                 auto pv) {
            using namespace std;
            cout << left << setw(10) << setfill(fill) << positionId     << " ";
            cout << left << setw(12) << setfill(fill) << instrumentId   << " ";
            cout << left << setw(18) << setfill(fill) << instrumentKind << " ";
            cout << left << setw(10) << setfill(fill) << ccy            << " ";
            cout << left << setw(11) << setfill(fill) << pv             << " ";
            cout << '\n';
        };

        printRow(' ', "PositionId", "InstrumentId", "InstrumentKind", "PVCurrency", "PV");
        printRow('-', "", "", "", "", "");
        for (const auto& [positionId, position] : p.positions) {
            const auto instrumentId = position.instrument;
            const auto instrumentKind = name(p.instruments.at(instrumentId)->kind());
            const auto& pv_ccy = [&instrumentPvs, &instrumentId]() -> 
                std::optional<std::pair<double,Currency>> {
                if (const auto i = instrumentPvs.find(instrumentId); i != instrumentPvs.end()) {
                    return {i->second};
                } else {
                    return std::nullopt;
                }
            }();
            if (pv_ccy) {
                const auto& [instrumentPv,ccy] = *pv_ccy;
                const auto positionPv = position.notional * instrumentPv;
                printRow(' ', positionId, instrumentId, instrumentKind, name(ccy), positionPv);
            } else {
                printRow(' ', positionId, instrumentId, instrumentKind, "NA", "NA");
            }
        }
    }

}
