#include "GeneralPricer.hpp"
#include "Pricers/PricingConfiguration.hpp"
#include "Container/Container.hpp"

GeneralPricer::GeneralPricer(const Container& container,
                             const std::vector<InstrumentId>& instruments,
                             const PricingConfiguration& config)
{
    std::map<PricerKind,std::vector<InstrumentId>> kind_instruments;
    for (const auto& instrument : instruments) {
        const auto pricerKind = config.pricerKind( *container.get(instrument) );
        kind_instruments[pricerKind].push_back(instrument);
    }
    for (auto& [kind, ids] : kind_instruments)
        pricers_.push_back( PricerId{ kind, std::move(ids) } );
}

std::vector<VariantId> GeneralPricer::precedents() const
{
    return std::vector<VariantId>(pricers_.begin(),pricers_.end());
}

std::map<InstrumentId,PV> GeneralPricer::pvs(const Container& container) const
{
    std::map<InstrumentId,PV> pvs;
    for (const auto& pricer: pricers_)
        pvs.merge(container.get(pricer)->pvs(container));
    return pvs;
}
