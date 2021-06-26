#include "Factory.hpp"

class S3ModelSubFactory : public SubFactory<S3ModelId> {
public:
    std::vector<VariantId> getPrecedents(const IdType& id, const Container& container) const override;
    std::unique_ptr<ObjectType> make(const IdType& id, const Container& container) const override;
};
