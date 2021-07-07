#include "PV.hpp"
#include "Container/Container.hpp"
#include "Pricers/Pricer.hpp"

namespace Metrics {

std::map<InstrumentId,Result> PV::compute(const Container& container) const
{
    std::map<InstrumentId,Result> results;

    const auto pricerIds = container.ids<PricerId>();
    for (const auto& pricerId : pricerIds) {
        if (pricerId.kind == PricerKind::General)
            continue;
        
        const auto& pricer = *container.get(pricerId);
        const auto pvs = pricer.pvs(container);
        for (const auto& [instrumentId, pv] : pvs) {
            Result pvResult{ std::pair{ PVKey{pv.ccy}, pv.value } };
            results.emplace(instrumentId,std::move(pvResult));
        }        
    }
    return results;
}

}
