#include "PVImpl.hpp"
#include "Container/Container.hpp"
#include "Pricers/Pricer.hpp"

std::map<InstrumentId,Result> PVImpl::compute(const PricerId& pricerId,
                                              const Container& container) const
{
    const auto& pricer = *container.get(pricerId);
    const auto pvs = pricer.pvs(container);
    std::map<InstrumentId,Result> results;
    for (const auto& [instrumentId, pv] : pvs) {
        Result pvResult{ std::pair{ PVKey{pv.ccy}, pv.value } };
        results.emplace(instrumentId,std::move(pvResult));
    }
    return results;
}
