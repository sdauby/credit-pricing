#pragma once
#include <map>

template<typename ObjectIdT>
using ObjectType = typename ObjectIdT::ObjectType;

template<typename ObjectIdT>
using ObjectPtr = std::unique_ptr<const typename ObjectIdT::ObjectType>;

class Container final {
public:
    Container();
    explicit Container(const Container* baseContainer);
    ~Container();

    template<typename ObjectIdT>
    const ObjectType<ObjectIdT>* get(const ObjectIdT& id) const;

    template<typename ObjectIdT>
    void set(const ObjectIdT& id, ObjectPtr<ObjectIdT>&& model);

    template<typename ObjectIdT>
    std::vector<ObjectIdT> ids() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
