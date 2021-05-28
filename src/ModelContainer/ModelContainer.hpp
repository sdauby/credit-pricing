#pragma once
#include <map>

template<typename ModelIdT>
using ModelType = typename ModelIdT::ModelType;

template<typename ModelIdT>
using ModelPtr = std::unique_ptr<const typename ModelIdT::ModelType>;

class ModelContainer final {
public:
    ModelContainer();
    ~ModelContainer();

    template<typename ModelIdT>
    const ModelType<ModelIdT>* get(const ModelIdT& id) const;

    template<typename ModelIdT>
    void set(const ModelIdT& id, ModelPtr<ModelIdT>&& model);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
