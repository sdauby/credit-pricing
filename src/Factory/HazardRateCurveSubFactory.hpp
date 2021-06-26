#include "Factory.hpp"

class HazardRateCurveSubFactory : public SubFactory<HazardRateCurveId> {
public:
    std::vector<VariantId> getPrecedents(const IdType& id, const Container& container) const override;
    std::unique_ptr<ObjectType> make(const IdType& id, const Container& container) const override;
};
