#include "IRDeltaImpl.hpp"
#include "Container/Container.hpp"
#include "Mutations/IRCurveMutation.hpp"
#include "Mutations/UpdateMutation.hpp"
#include "Pricers/Pricer.hpp"

IRDeltaImpl::IRDeltaImpl(const ContainerDag& updateDag, const Factory& factory) :
    updateDag_(updateDag),
    factory_(factory)
{}

std::map<InstrumentId,Result> IRDeltaImpl::compute(const Pricer& pricer,
                                                   const Container& container) const
{
    std::map<InstrumentId,Result> results;

    for (const auto& curveId : container.ids<InterestRateCurveId>()) {
        constexpr auto shiftSize = 1e-8;
        constexpr auto shifts = std::array{+shiftSize,-shiftSize};
        constexpr auto scale = 1e-4;
        std::array<std::map<InstrumentId,PV>,2> pvs;
        for (auto i : {0,1}) {
            const auto container1 = IRCurveMutation(curveId,shifts[i]).apply(container);
            const auto container2 = UpdateMutation(std::vector<VariantId>{curveId},updateDag_,factory_).apply(*container1);
            pvs[i] = pricer.pvs(*container2);
        }
        for (auto i0=pvs[0].cbegin(), e0=pvs[0].cend(),
                  i1=pvs[1].cbegin(), e1=pvs[1].cend(); i0!=e0; ++i0, ++i1) {
            assert(i1!=e1);

            const auto& [instrumentId0, pv0] = *i0;
            const auto& [instrumentId1, pv1] = *i1;
            
            assert(instrumentId0 == instrumentId1);
            assert(pv0.ccy == pv1.ccy);

            auto deltaKey = IRDeltaKey{ .ccy = pv0.ccy, .curveId = curveId };
            const auto delta = (pv0.value-pv1.value) / (2*shiftSize) * scale;
            results[instrumentId0].emplace( std::pair{std::move(deltaKey),delta} );
        }
    }
    return results;
}
