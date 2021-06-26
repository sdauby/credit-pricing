#include "Factory.hpp"

class PricingConfiguration;

class PricerSubFactory : public SubFactory<PricerId> {
public:
    explicit PricerSubFactory(const PricingConfiguration& config);
    std::vector<VariantId> getPrecedents(const IdType& id, const Container& container) const override;
    std::unique_ptr<ObjectType> make(const IdType& id, const Container& container) const override;
private:
    const PricingConfiguration& config_;
};
