#include "IRDeltaImpl.hpp"
#include <iostream>

std::map<InstrumentId,Result> IRDeltaImpl::compute(const Pricer& pricer,
                                                   const Container& modelContainer) const
{
    std::map<InstrumentId,Result> results;
    for (const auto& curveId : modelContainer.ids<InterestRateCurveId>()) {
        constexpr auto shiftSize = 1e-8;
        constexpr auto shifts = std::array{+shiftSize,-shiftSize};
        std::array<std::map<InstrumentId,PV>,2> pvs;
        for (auto i : {0,1}) {
            const auto container = IRCurveMutation(curveId,shifts[i]).apply(modelContainer);
            pvs[i] = pricer.pvs(*container);
        }
        for (auto i0=pvs[0].cbegin(), e0=pvs[0].cend(),
                  i1=pvs[1].cbegin(), e1=pvs[1].cend(); i0!=e0; ++i0, ++i1) {
            assert(i1!=e1);

            const auto& [instrumentId0, pv0] = *i0;
            const auto& [instrumentId1, pv1] = *i1;
            
            assert(instrumentId0 == instrumentId1);
            assert(pv0.ccy == pv1.ccy);

            auto deltaKey = IRDeltaKey{ .ccy = pv0.ccy, .curveId = curveId };
            const auto delta = (pv0.value-pv1.value)/(2*shiftSize);
            results[instrumentId0].emplace( std::pair{std::move(deltaKey),delta} );
        }
    }
    return results;
}
