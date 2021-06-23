#include <iostream>
#include <set>
#include "Core/Currency.hpp"
#include "Core/Issuer.hpp"
#include "Core/Date.hpp"
#include "Core/Data.hpp"
#include "Core/Tabulator.hpp"
#include "Instruments/Portfolio.hpp"
#include "Instruments/Cds.hpp"
#include "Instruments/FixedCouponBond.hpp"
#include "Instruments/FloatingCouponBond.hpp"
#include "Instruments/IRSwap.hpp"
#include "ModelFactory/ModelFactory.hpp"
#include "Metrics/Metric.hpp"
#include "Metrics/Result.hpp"
#include "PricingEngine/PricingConfiguration.hpp"
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
        p.instruments.emplace(0, make<Cds>(Ccy::EUR, BNP, std::vector<Date>{ 0y, 1y, 2y, 3y, 4y, 5y, }, 0.01));
        p.positions.emplace(0, Position{ .notional =  1'000'000, .instrument = 0 });
        p.positions.emplace(1, Position{ .notional = -1'000'000, .instrument = 0 });

        p.instruments.emplace(1, make<Cds>(Ccy::USD, JPM, std::vector<Date>{ 2y, 3y, 4y, 5y, }, 0.02));
        p.positions.emplace(2, Position{ .notional = 2'000'000, .instrument = 1 });

        p.instruments.emplace(2, make<FixedCouponBond>(Ccy::USD, C, std::vector<Date>{ 0y, 1y, 2y, 3y, }, 0.01));
        p.positions.emplace(3, Position{ .notional = 1'000'000, .instrument = 2 });

        p.instruments.emplace(3, make<FixedCouponBond>(Ccy::EUR, BNP, std::vector<Date>{ 2y, 4y, 6y, }, 0.02));
        p.positions.emplace(4, Position{ .notional = 1'000'000, .instrument = 3 });

        p.instruments.emplace(4, make<FloatingCouponBond>(Ccy::USD, JPM, std::vector<Date>{ 0y, 1y, 2y, 3y, }, 0.01));
        p.positions.emplace(5, Position{ .notional = 1'000'000, .instrument = 4 });

        p.instruments.emplace(5,make<FloatingCouponBond>(Ccy::USD, C, std::vector<Date>{ 0y, 2y, 4y, 6y, }, -0.02));
        p.positions.emplace(6, Position{ .notional = 1'000'000, .instrument = 5 });

        p.instruments.emplace(6,make<IRSwap>(Ccy::USD, std::vector<Date>{ 0y, 1y, 2y, 3y, 4y, 5y}, 0.02));
        p.positions.emplace(7, Position{ .notional = 1'000'000, .instrument = 6 });

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
            t.addCell( std::to_string(instrumentId) );
            t.addCell( name( p.instruments.at(instrumentId)->kind() ) );
            t.addCell( name( config.pricerKind( *p.instruments.at(instrumentId) ) ) );

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