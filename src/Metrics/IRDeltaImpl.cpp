#include "IRDeltaImpl.hpp"
#include "Container/Container.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Pricers/Pricer.hpp"
#include "UpdatePropagation.hpp"
#include "Elaboration/ObjectTypes.hpp"

namespace {

std::unique_ptr<Container> applyRateShift(const Container& container, const InterestRateCurveId& id, double shift)
{
    const auto& curve = *container.get(id);
    auto curve_ = curve.applyRateShift(shift);
    auto overlay = std::make_unique<Container>(container);
    overlay->set(id,std::move(curve_));
    return overlay;
}

}

IRDeltaImpl::IRDeltaImpl(const IdDagAux&& requests, const ElaboratorGeneralFactory& factory) :
    requests_(std::move(requests)),
    factory_(factory)
{}

std::map<InstrumentId,Result> IRDeltaImpl::compute(const PricerId& pricerId,
                                                   const Container& container) const
{
    std::map<InstrumentId,Result> results;
    const auto& pricer = *container.get(pricerId);

    const UpdateFunction update = 
        [&factory = factory_] (Container& container, const VariantId& id, const std::vector<VariantId>& dirtyPrecedents) {
            std::visit( [&] (const auto& id) -> void {
                const auto elaborator = factory.make(id);
                while (!elaborator->getRequestBatch(container).empty()) {}
                auto newObject = elaborator->make(container);
                container.set(id,std::move(newObject));
            }, id);
        };

    for (const auto& curveId : container.ids<InterestRateCurveId>()) {
        constexpr auto shiftSize = 1e-8;
        constexpr auto shifts = std::array{+shiftSize,-shiftSize};
        constexpr auto scale = 1e-4;
        std::array<std::map<InstrumentId,PV>,2> pvs;
        for (auto i : {0,1}) {
            const auto container1 = applyRateShift(container,curveId,shifts[i]);
            const auto container2 = propagateUpdate(*container1,std::vector<VariantId>{curveId},requests_,update);
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
