#pragma once
#include <map>

template<typename ObjectIdT>
using ObjectType = typename ObjectIdT::ObjectType;

template<typename ObjectIdT>
using ObjectPtr = std::unique_ptr<const typename ObjectIdT::ObjectType>;

template<typename ObjectIdT>
using ObjectMap = std::map<ObjectIdT,ObjectPtr<ObjectIdT>>;


class Container final {
public:
    ~Container();
    Container();
    Container(Container&& container);
    Container& operator=(Container&& container);
    explicit Container(const Container& baseContainer); // This is not a copy CTOR. Container is move-only.

    template<typename ObjectIdT>
    const ObjectType<ObjectIdT>* get(const ObjectIdT& id) const;

    template<typename ObjectIdT>
    void set(const ObjectIdT& id, ObjectPtr<ObjectIdT>&& object);

    template<typename ObjectIdT>
    std::vector<ObjectIdT> ids() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
