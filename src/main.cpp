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
#include "PricingEngine/PricingConfiguration.hpp"
#include "PricingEngine/PricingEngine.hpp"

namespace {
    template<typename InstrumentType>
    const auto make = [](auto&&... xs) {
        using DataType = typename InstrumentType::DataType;
        return std::unique_ptr<Instrument>{new InstrumentType{DataType{std::forward<decltype(xs)>(xs)...}}};
    };
}

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

    return p;
}

int main() {
    const auto p = getSamplePortfolio();

    std::cout << "Using the S3 pricer:\n";
    PricingEngine::price(p,PricingConfiguration{PricerKind::S3});
    std::cout << "\n";

    std::cout << "Using the IR pricer:\n";
    PricingEngine::price(p,PricingConfiguration{PricerKind::IR});
    std::cout << "\n";

    return 0;
}