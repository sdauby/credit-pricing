#pragma once
#include <map>

template<typename IdT>
using Object = typename IdT::ObjectType;

class Container final {
public:
    ~Container();
    Container();
    Container(Container&& container);
    Container& operator=(Container&& container);
    explicit Container(const Container& baseContainer); // This is not a copy CTOR. Container is move-only.

    template<typename IdT>
    Object<IdT>* get(const IdT& id) const;

    template<typename IdT>
    void set(const IdT& id, std::unique_ptr<Object<IdT>>&& object);

    template<typename IdT>
    std::vector<IdT> ids() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
