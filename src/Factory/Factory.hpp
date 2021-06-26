#pragma once
#include <vector>
#include "Container/VariantId.hpp"
#include "Container/Container.hpp"

template<typename IdT>
class SubFactory {
public:
    using IdType = IdT;
    using ObjectType = Object<IdT>;

    virtual ~SubFactory() = default;
    SubFactory() = default;
    SubFactory(const SubFactory&) = delete;
    SubFactory& operator=(const SubFactory&) = delete;

    virtual std::vector<VariantId> getPrecedents(const IdType& id, const Container& container) const = 0;
    virtual std::unique_ptr<ObjectType> make(const IdType& id, const Container& container) const = 0;
};

class Factory final {
public:
    ~Factory();
    Factory();

    template<typename IdT>
    void setSubFactory(std::unique_ptr<SubFactory<IdT>> subFactory);

    template<typename IdT>
    std::vector<VariantId> getPrecedents(const IdT& id, const Container& container) const
    {
        return subFactory<IdT>().getPrecedents(id,container);
    }

    template<typename IdT>
    std::unique_ptr<Object<IdT>> make(const IdT& id, const Container& container) const
    {
        return subFactory<IdT>().make(id,container);
    }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    template<typename IdT>
    const SubFactory<IdT>& subFactory() const;
};
