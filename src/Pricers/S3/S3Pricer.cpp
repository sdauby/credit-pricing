#include "Pricers/S3/S3Pricer.hpp"

#include "Container/Container.hpp"
#include "Pricers/S3/CdsPricing.hpp"
#include "Pricers/S3/FixedCouponBondPricing.hpp"
#include "Pricers/S3/FloatingCouponBondPricing.hpp"

#include <set>

namespace {

    template<typename InstrumentType>
    class S3UnitPricerImpl : public S3UnitPricer {
    public:
        explicit S3UnitPricerImpl(const InstrumentType& instrument) : instrument_{instrument} {}
        S3ModelId requiredModel() const override { return ::modelId(instrument_); };
        double pv(const S3Model& model) const override { return ::pv(instrument_, model); }
    private:
        const InstrumentType& instrument_;
    };

    template<typename InstrumentType>
    auto makeS3UnitPricer_(const Instrument& instrument)
    {   
        const auto& i = static_cast<const InstrumentType&>(instrument);
        return std::unique_ptr<S3UnitPricer>(new S3UnitPricerImpl{i});
    };

    std::unique_ptr<S3UnitPricer> makeS3UnitPricer(const Instrument& instrument)
    {
        switch (instrument.kind()) {
            case Cds               ::instrumentKind : return makeS3UnitPricer_<Cds               >(instrument);
            case FixedCouponBond   ::instrumentKind : return makeS3UnitPricer_<FixedCouponBond   >(instrument);
            case FloatingCouponBond::instrumentKind : return makeS3UnitPricer_<FloatingCouponBond>(instrument);
            default:
                assert(false && "Instrument kind not supported by S3Pricer");
                return {};
        }
    }

    // Aggregate the tenor structures of ids having the same ccy/issuer.
    // Substitute the aggregated tenor structures in the original model ids.
    void aggregateTenorStructures(std::map<InstrumentId,S3ModelId>& s3ModelIds)
    {
        const auto aggregationKey = [] (const S3ModelId& id) {
            return std::tuple{id.ccy,id.issuer};
        };
        using KeyType = decltype(aggregationKey(S3ModelId{}));

        std::map<KeyType,std::set<Date>> aggTenorStructures;
        for (const auto& [instrumentId,s3ModelId] : s3ModelIds) {
            const auto& aggKey = aggregationKey(s3ModelId);
            const auto& ts = s3ModelId.tenorStructure;
            aggTenorStructures[aggKey].insert(ts.cbegin(),ts.cend());
        }

        for (auto& [instrumentId,s3ModelId] : s3ModelIds) {
            const auto& aggKey = aggregationKey(s3ModelId);
            const auto& ts = aggTenorStructures[aggKey];
            s3ModelId.tenorStructure.assign(ts.cbegin(),ts.cend());
        }
    }

}

S3Pricer::S3Pricer(const Container& container, const std::vector<InstrumentId>& instrumentIds)
{
    for (const auto& instrumentId : instrumentIds) {
        auto pricer = makeS3UnitPricer(*container.get(instrumentId));
        s3ModelIds_.emplace(instrumentId,pricer->requiredModel());
        unitPricers_.emplace(instrumentId,std::move(pricer));
    }
    aggregateTenorStructures(s3ModelIds_);
}

std::vector<S3ModelId> S3Pricer::requiredModels() const
{
    std::set<S3ModelId> s3ModelIds;
    for (const auto& [instrumentId,s3ModelId] : s3ModelIds_)
        s3ModelIds.insert(s3ModelId);

    return std::vector<S3ModelId>(s3ModelIds.cbegin(),s3ModelIds.cend());
}

std::map<InstrumentId,PV> S3Pricer::pvs(const Container& modelContainer) const 
{
    std::map<InstrumentId,PV> instrumentPvs;
    for (const auto& [instrumentId,unitPricer] : unitPricers_) {
        const auto& s3ModelId = s3ModelIds_.at(instrumentId);
        const auto* model = modelContainer.get(s3ModelId);
        assert(model);
        const auto pv = unitPricer->pv(*model);
        instrumentPvs.emplace(instrumentId,PV{pv,s3ModelId.ccy});
    }
    return instrumentPvs;
}
