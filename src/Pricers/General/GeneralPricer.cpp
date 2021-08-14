#include "Pricers/General/GeneralPricer.hpp"

#include "Core/overloaded.hpp"
#include "Container/Container.hpp"
#include "Pricers/PricingConfiguration.hpp"
#include "Pricers/IR/IRPricer.hpp"
#include "Pricers/S3/S3Pricer.hpp"

GeneralPricer::GeneralPricer(const Container& container,
                             const std::vector<InstrumentId>& instruments,
                             const PricingConfiguration& config)
{
    using GroupingKey = std::variant<InterestRateCurveId,S3ModelId>;

    const auto groupingKey = [&] (const InstrumentId& instrumentId) -> GroupingKey {
        const auto& instrument = *container.get(instrumentId);
        const auto pricerKind = config.pricerKind(instrument);

        switch (pricerKind) {
            case PricerKind::S3: {
                const auto pricer = S3Pricer( container, { instrumentId } );
                const auto models = pricer.requiredModels();
                assert(models.size() == 1);
                auto s3ModelId = models.back();
                s3ModelId.tenorStructure.clear();
                return s3ModelId;
            }
            case PricerKind::IR: {
                const auto pricer = IRPricer( container, { instrumentId } );
                const auto curves = pricer.requiredCurves();
                assert(curves.size() == 1);
                return curves.back();
            }
            case PricerKind::General:
                assert(!"bad pricer kind");
                return {};
        }
    };

    auto groups = std::map<GroupingKey,std::vector<InstrumentId>>{};
    for (const auto& instrument : instruments)
        groups[groupingKey(instrument)].push_back(instrument);

    for (auto& [key,instruments] : groups) {
        const auto pricerKind = std::visit(
            overloaded {
                [] (const InterestRateCurveId&) { return PricerKind::IR; },
                [] (const S3ModelId&) { return PricerKind::S3; }
            },
            key);
        pricers_.push_back( PricerId{ pricerKind, std::move(instruments) } );
    }
}

std::vector<PricerId> GeneralPricer::requiredPricers() const
{
    return pricers_;
}

std::map<InstrumentId,PV> GeneralPricer::pvs(const Container& container) const
{
    std::map<InstrumentId,PV> pvs;
    for (const auto& pricer: pricers_)
        pvs.merge(container.get(pricer)->pvs(container));
    return pvs;
}
