#include <iomanip>
#include <iostream>
#include "Core/Currency.hpp"
#include "Core/Issuer.hpp"
#include "Core/Date.hpp"
#include "Core/Data.hpp"
#include "Instruments/Portfolio.hpp"
#include "Instruments/Cds.hpp"
#include "Instruments/FixedCouponBond.hpp"
#include "Instruments/FloatingCouponBond.hpp"
#include "Instruments/IRSwap.hpp"
#include "ModelFactory/ModelFactory.hpp"
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

    void outputPvs(const Portfolio& p, const auto& instrumentPvs)
    {
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

int main() {
    const auto p = getSamplePortfolio();
    ModelFactory modelFactory;

    std::cout << "Using the S3 pricer:\n";
    auto s3Pvs = PricingEngine::price(p.instruments,PricingConfiguration{PricerKind::S3}, modelFactory);
    outputPvs(p,s3Pvs);
    std::cout << "\n";

    std::cout << "Using the IR pricer:\n";
    auto irPvs = PricingEngine::price(p.instruments,PricingConfiguration{PricerKind::IR}, modelFactory);
    outputPvs(p,irPvs);
    std::cout << "\n";

    return 0;
}