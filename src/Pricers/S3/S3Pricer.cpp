#include "S3Pricer.hpp"
#include "CdsPricing.hpp"
#include "FixedCouponBondPricing.hpp"
#include "FloatingCouponBondPricing.hpp"
#include "ModelContainer/ModelContainer.hpp"
#include <set>

namespace {

    template<typename InstrumentType>
    class S3UnitPricerImpl : public S3UnitPricer {
    public:
        explicit S3UnitPricerImpl(const InstrumentType& instrument) : instrument_{instrument} {}
        S3ModelId modelId() const override { return ::modelId(instrument_); };
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

S3Pricer::S3Pricer(const InstrumentMap& instruments, const std::vector<InstrumentId>& instrumentIds)
{
    for (const auto& instrumentId : instrumentIds) {
        auto pricer = makeS3UnitPricer(*instruments.at(instrumentId));
        s3ModelIds_.emplace(instrumentId,pricer->modelId());
        unitPricers_.emplace(instrumentId,std::move(pricer));
    }
    aggregateTenorStructures(s3ModelIds_);
}

std::vector<ModelId> S3Pricer::requiredModels() const
{
    std::set<S3ModelId> s3ModelIds;
    for (const auto& [instrumentId,s3ModelId] : s3ModelIds_)
        s3ModelIds.insert(s3ModelId);
    
    std::vector<ModelId> modelIds;
    modelIds.reserve(s3ModelIds.size());
    for (auto& s3ModelId : s3ModelIds)
         modelIds.emplace_back(std::move(s3ModelId));
    return modelIds;
}

PvResult S3Pricer::pvs(const ModelContainer& modelContainer) const 
{
    PvResult instrumentPvs;
    for (const auto& [instrumentId,unitPricer] : unitPricers_) {
        const auto& s3ModelId = s3ModelIds_.at(instrumentId);
        const auto* model = modelContainer.get(s3ModelId);
        assert(model!=nullptr);
        const auto pv = unitPricer->pv(*model);
        const auto ccy = s3ModelId.ccy;
        instrumentPvs.emplace(instrumentId,std::pair{pv,ccy});
    }
    return instrumentPvs;
}
