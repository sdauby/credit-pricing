#include "Factory.hpp"

class InstrumentSubFactory : public SubFactory<InstrumentId> {
public:
    std::vector<VariantId> getPrecedents(const IdType& id, const Container& container) const override;
    std::unique_ptr<ObjectType> make(const IdType& id, const Container& container) const override;
};
