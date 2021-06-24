#include "PVImpl.hpp"

std::map<InstrumentId,Result> PVImpl::compute(const Pricer& pricer,
                                              const Container& modelContainer) const
{
    const auto pvs = pricer.pvs(modelContainer);
    std::map<InstrumentId,Result> results;
    for (const auto& [instrumentId, pv] : pvs) {
        Result pvResult{ std::pair{ PVKey{pv.ccy}, pv.value } };
        results.emplace(instrumentId,std::move(pvResult));
    }
    return results;
}
