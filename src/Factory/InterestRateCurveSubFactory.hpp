#include "Factory.hpp"

class InterestRateCurveSubFactory : public SubFactory<InterestRateCurveId> {
public:
    std::vector<VariantId> getPrecedents(const IdType& id, const Container& container) const override;
    std::unique_ptr<ObjectType> make(const IdType& id, const Container& container) const override;
};
