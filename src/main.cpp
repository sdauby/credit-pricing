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
#include "ModelFactory/ModelFactory.hpp"
#include "Metrics/Metric.hpp"
#include "Metrics/Result.hpp"
#include "PricingEngine/PricingConfiguration.hpp"
#include "PricingEngine/Portfolio.hpp"
#include "PricingEngine/PricingEngine.hpp"

namespace {
    Portfolio getSamplePortfolio()
    {
        Portfolio p;

        using Ccy = Currency;
        using namespace Data::Issuers;
        using namespace std::chrono;

        // Create instruments with specified InstrumentIds.
        // Create positions on those instruments with specified PositionIds.
        const auto iid = [] (unsigned k) { return InstrumentId{ .k = k }; };

        p.instruments.set(iid(0), make<Cds>(Ccy::EUR, BNP, std::vector<Date>{ 0y, 1y, 2y, 3y, 4y, 5y, }, 0.01));
        p.positions.emplace(0, Position{ .notional =  1'000'000, .instrument = iid(0) });
        p.positions.emplace(1, Position{ .notional = -1'000'000, .instrument = iid(0) });

        p.instruments.set(iid(1), make<Cds>(Ccy::USD, JPM, std::vector<Date>{ 2y, 3y, 4y, 5y, }, 0.02));
        p.positions.emplace(2, Position{ .notional = 2'000'000, .instrument = iid(1) });

        p.instruments.set(iid(2), make<FixedCouponBond>(Ccy::USD, C, std::vector<Date>{ 0y, 1y, 2y, 3y, }, 0.01));
        p.positions.emplace(3, Position{ .notional = 1'000'000, .instrument = iid(2) });

        p.instruments.set(iid(3), make<FixedCouponBond>(Ccy::EUR, BNP, std::vector<Date>{ 2y, 4y, 6y, }, 0.02));
        p.positions.emplace(4, Position{ .notional = 1'000'000, .instrument = iid(3) });

        p.instruments.set(iid(4), make<FloatingCouponBond>(Ccy::USD, JPM, std::vector<Date>{ 0y, 1y, 2y, 3y, }, 0.01));
        p.positions.emplace(5, Position{ .notional = 1'000'000, .instrument = iid(4) });

        p.instruments.set(iid(5),make<FloatingCouponBond>(Ccy::USD, C, std::vector<Date>{ 0y, 2y, 4y, 6y, }, -0.02));
        p.positions.emplace(6, Position{ .notional = 1'000'000, .instrument = iid(5) });

        p.instruments.set(iid(6),make<IRSwap>(Ccy::USD, std::vector<Date>{ 0y, 1y, 2y, 3y, 4y, 5y}, 0.02));
        p.positions.emplace(7, Position{ .notional = 1'000'000, .instrument = iid(6) });

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
            t.addCell( std::to_string(positionId) );
            t.addCell( std::to_string(instrumentId.k) );
            t.addCell( name( p.instruments.get(instrumentId)->kind() ) );
            t.addCell( name( config.pricerKind( *p.instruments.get(instrumentId) ) ) );

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
    const auto p = getSamplePortfolio();

    const auto metrics = std::vector<Metric> {Metric::PV, Metric::IRDelta};
    //const auto metrics = std::vector<Metric> {Metric::PV};

    for (const auto preferredPricer : { PricerKind::S3, PricerKind::IR }) {
        std::cout << "Prefer the " << name(preferredPricer) << " pricer:\n";
        const auto config = PricingConfiguration(preferredPricer);
        auto results = PricingEngine::run(p.instruments,config,metrics);
        outputMetrics(p,config,std::move(results));
        std::cout << '\n';
    }

    return 0;
}