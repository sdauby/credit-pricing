#include "IRPricer.hpp"
#include "Instruments/Instrument.hpp"
#include "Instruments/FixedCouponBond.hpp"
#include "Instruments/FloatingCouponBond.hpp"
#include "Instruments/IRSwap.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/InterestRateCurve/IRSwapAnalytics.hpp"
#include "Container/Container.hpp"
#include "Container/InterestRateCurveId.hpp"
#include <set>

namespace {

    template <typename InstrumentT>
    class IRUnitPricerImpl : public IRUnitPricer {
    public:
        explicit IRUnitPricerImpl(const InstrumentT& instrument) : instrument_{instrument} {}
        InterestRateCurveId requiredCurve() const override;
        double pv(const InterestRateCurve& curve) const override;
    private:
        const InstrumentT& instrument_;
    };

    template<>
    InterestRateCurveId IRUnitPricerImpl<FixedCouponBond>::requiredCurve() const 
    {
        return { .ccy = instrument_.data().ccy };
    }

    template<>
    double IRUnitPricerImpl<FixedCouponBond>::pv(const InterestRateCurve & curve) const
    {
        const auto df = [&curve](Date t) {
            using namespace std::chrono;
            constexpr auto T0 = 0y;
            return curve.discountFactor(T0,t);
        };

        const auto& data = instrument_.data();
        const auto c = data.c;
        const auto& t = data.t;
        const auto N = t.size()-1;
        auto pv = 0.0;
        
        // fixed coupons
        for (auto n=1; n<=N; ++n) {
            const auto d = dayCountFactor(t[n-1],t[n]);
            pv += c * d * df(t[n]);
        }

        // principal
        pv += df(t[N]);
        
        return pv;
    }

    template<>
    InterestRateCurveId IRUnitPricerImpl<FloatingCouponBond>::requiredCurve() const 
    {
        return { .ccy = instrument_.data().ccy };
    }

    template<>
    double IRUnitPricerImpl<FloatingCouponBond>::pv(const InterestRateCurve & curve) const
    {
        const auto df = [&curve](Date t) {
            using namespace std::chrono;
            constexpr auto T0 = 0y;
            return curve.discountFactor(T0,t);
        };

        const auto& data = instrument_.data();
        const auto s = data.s;
        const auto& t = data.t;
        const auto N = t.size()-1;
        auto pv = 0.0;

        // Libors
        pv += df(t[0]) - df(t[N]);
        
        // spreads
        for (auto n=1; n<=N; ++n) {
            const auto d = dayCountFactor(t[n-1],t[n]);
            pv += s * d * df(t[n]);
        }

        // principal
        pv += df(t[N]);

        return pv;
    }

    template<>
    InterestRateCurveId IRUnitPricerImpl<IRSwap>::requiredCurve() const 
    {
        return { .ccy = instrument_.data().ccy };
    }

    template<>
    double IRUnitPricerImpl<IRSwap>::pv(const InterestRateCurve & curve) const
    {
        const auto df = [&curve](Date t) {
            using namespace std::chrono;
            constexpr auto T0 = 0y;
            return curve.discountFactor(T0,t);
        };

        const auto& data = instrument_.data();
        const auto K = data.K;
        const auto& t = data.t;
        const auto N = t.size()-1;
        auto pv = 0.0;

        // Libors
        pv -= df(t[0]) - df(t[N]);
        
        // fixed rates
        pv += K * annuity(instrument_,curve);

        return pv;
    }


    template<typename InstrumentT>
    std::unique_ptr<IRUnitPricer> makeIRUnitPricer_(const Instrument& instrument) 
    {
        const auto& i = static_cast<const InstrumentT&>(instrument);
        return std::unique_ptr<IRUnitPricer>(new IRUnitPricerImpl{i});
    }

    std::unique_ptr<IRUnitPricer> makeIRUnitPricer(const Instrument& instrument) 
    {
        switch (instrument.kind()) {
            case FixedCouponBond   ::instrumentKind : return makeIRUnitPricer_<FixedCouponBond   >(instrument);
            case FloatingCouponBond::instrumentKind : return makeIRUnitPricer_<FloatingCouponBond>(instrument);
            case IRSwap            ::instrumentKind : return makeIRUnitPricer_<IRSwap            >(instrument);
            default: 
                assert(false && "Instrument kind not supported by IRPricer");
                return {};
        }
    }

}

IRPricer::IRPricer(const InstrumentMap& instruments, const std::vector<InstrumentId>& instrumentIds) 
{
    for (const auto& instrumentId : instrumentIds) {
        auto pricer = makeIRUnitPricer(*instruments.at(instrumentId));
        unitPricers_.emplace(instrumentId,std::move(pricer));
    }
}

std::vector<ModelId> IRPricer::requiredModels() const
{ 
    std::set<InterestRateCurveId> irCurveIds;
    for (const auto& [instrumentId,pricer] : unitPricers_)
        irCurveIds.insert(pricer->requiredCurve());
    
    std::vector<ModelId> ids;
    ids.reserve(irCurveIds.size());
    for (auto& irCurveId : irCurveIds)
         ids.emplace_back(std::move(irCurveId));
    return ids;
}
    
std::map<InstrumentId,PV> IRPricer::pvs(const Container& modelContainer) const 
{ 
    std::map<InstrumentId,PV> pvs;
    for (const auto& [instrumentId,pricer] : unitPricers_) {
        const auto& irCurveId = pricer->requiredCurve();
        const auto* irCurve = modelContainer.get(irCurveId);
        assert(irCurve);
        pvs.emplace(instrumentId,PV{pricer->pv(*irCurve), irCurveId.ccy});
    }
    return pvs;
}
