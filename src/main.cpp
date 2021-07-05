#include <iostream>
#include <set>
#include "Core/Currency.hpp"
#include "Core/Issuer.hpp"
#include "Core/Date.hpp"
#include "Core/Data.hpp"
#include "Core/Tabulator.hpp"
#include "Instruments/Cds.hpp"
#include "Instruments/FixedCouponBond.hpp"
#include "Instruments/FloatingCouponBond.hpp"
#include "Instruments/IRSwap.hpp"
#include "Metrics/Metric.hpp"
#include "Metrics/Result.hpp"
#include "Pricers/PricingConfiguration.hpp"
#include "PricingEngine/Portfolio.hpp"
#include "PricingEngine/PricingEngine.hpp"
#include "PricingEngine/Position.hpp"


namespace {
    Portfolio getSamplePortfolio()
    {
        Portfolio p;

        using Ccy = Currency;
        using namespace Data::Issuers;
        using namespace std::chrono;

        p.makeInstrument = [] (const InstrumentId& id) -> std::unique_ptr<Instrument> {
            switch(id.k) {
                case 0: return make<Cds>(Ccy::EUR, BNP, std::vector<Date>{ 0y, 1y, 2y, 3y, 4y, 5y, }, 0.01);
                case 1: return make<Cds>(Ccy::USD, JPM, std::vector<Date>{ 2y, 3y, 4y, 5y, }, 0.02);
                case 2: return make<FixedCouponBond>(Ccy::USD, C, std::vector<Date>{ 0y, 1y, 2y, 3y, }, 0.01);
                case 3: return make<FixedCouponBond>(Ccy::EUR, BNP, std::vector<Date>{ 2y, 4y, 6y, }, 0.02);
                case 4: return make<FloatingCouponBond>(Ccy::USD, JPM, std::vector<Date>{ 0y, 1y, 2y, 3y, }, 0.01);
                case 5: return make<FloatingCouponBond>(Ccy::USD, C, std::vector<Date>{ 0y, 2y, 4y, 6y, }, -0.02);
                case 6: return make<IRSwap>(Ccy::USD, std::vector<Date>{ 0y, 1y, 2y, 3y, 4y, 5y}, 0.02);
                default:
                    assert(!"bad instrument id");
            }
        };

        p.positions.emplace(PositionId{0}, Position{ .notional =  1'000'000, .instrument{0} });
        p.positions.emplace(PositionId{1}, Position{ .notional = -1'000'000, .instrument{0} });
        p.positions.emplace(PositionId{2}, Position{ .notional =  2'000'000, .instrument{1} });
        p.positions.emplace(PositionId{3}, Position{ .notional =  1'000'000, .instrument{2} });
        p.positions.emplace(PositionId{4}, Position{ .notional =  1'000'000, .instrument{3} });
        p.positions.emplace(PositionId{5}, Position{ .notional =  1'000'000, .instrument{4} });
        p.positions.emplace(PositionId{6}, Position{ .notional =  1'000'000, .instrument{5} });
        p.positions.emplace(PositionId{7}, Position{ .notional =  1'000'000, .instrument{6} });

        return p;
    }

    void outputMetrics(const Portfolio& p, 
                       const PricingConfiguration& config,
                       std::map<InstrumentId,Result>&& instrumentMetrics) 
    {
        std::set<ResultKey> resultKeys;
        for (const auto& [id,result] : instrumentMetrics) {
            for (const auto& [key,value] : result)
                resultKeys.insert(key);
        }

        Tabulator t;

        t.addHeader({"PositionId"});
        t.addHeader({"InstrumentId"});
        t.addHeader({"InstrumentKind"});
        t.addHeader({"Pricer"});
        for (const auto& key : resultKeys)
            t.addHeader(toStrings(key));
        
        for (const auto& [positionId, position] : p.positions) {
            const auto& instrumentId = position.instrument;
            const auto& notional = position.notional;
            const auto& metrics = instrumentMetrics.at(instrumentId);

            t.startRow();
            t.addCell( std::to_string(positionId.k) );
            t.addCell( std::to_string(instrumentId.k) );
            t.addCell( name( p.makeInstrument(instrumentId)->kind() ) );
            t.addCell( name( config.pricerKind( *p.makeInstrument(instrumentId) ) ) );

            for (const auto& key : resultKeys) {
                if (const auto i = metrics.find(key); i != metrics.end())
                    t.addCell( std::to_string(notional * i->second) );
                else
                    t.addCell("");
            }
        }
        t.output(std::cout);
    }
}

int main() {
    auto p = getSamplePortfolio();
    const auto instruments = [&p] () {
        std::set<InstrumentId> instruments;
        for (const auto& [positionId, position] : p.positions)
            instruments.insert(position.instrument);
        return std::vector<InstrumentId>(instruments.cbegin(),instruments.cend());
    } ();

    const auto metrics = std::vector<Metric> {Metric::PV, Metric::IRDelta};

    for (const auto preferredPricer : { PricerKind::S3, PricerKind::IR }) {
        std::cout << "Prefer the " << name(preferredPricer) << " pricer:\n";
        const auto config = PricingConfiguration(preferredPricer);
        auto results = PricingEngine::run(instruments,p.makeInstrument,config,metrics);
        outputMetrics(p,config,std::move(results));
        std::cout << '\n';
    }

    return 0;
}