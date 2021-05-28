#include "ModelContainer.hpp"
#include "Core/Data.hpp"
#include "InterestRateCurveId.hpp"
#include "HazardRateCurveId.hpp"
#include "S3ModelId.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"
#include "Models/HazardRateCurve/HazardRateCurve.hpp"
#include "Models/S3/S3Model.hpp"

namespace {

    template<typename ModelIdT>
    using ModelMap = std::map<ModelIdT,ModelPtr<ModelIdT>>;

}

struct ModelContainer::Impl {
    std::tuple<
        ModelMap<InterestRateCurveId>,
        ModelMap<HazardRateCurveId>,
        ModelMap<S3ModelId>
    > models;
};

ModelContainer::ModelContainer() : pImpl(std::make_unique<Impl>()) {}
ModelContainer::~ModelContainer() = default;

template<typename ModelIdT>
const ModelType<ModelIdT>* ModelContainer::get(const ModelIdT& id) const
{
    const auto& modelMap = std::get<ModelMap<ModelIdT>>(pImpl->models);
    const auto& i = modelMap.find(id);
    if (i==modelMap.end())
        return nullptr;
    else
        return i->second.get();
}

template const ModelType<InterestRateCurveId>* ModelContainer::get(const InterestRateCurveId& id) const;
template const ModelType<HazardRateCurveId>* ModelContainer::get(const HazardRateCurveId& id) const;
template const ModelType<S3ModelId>* ModelContainer::get(const S3ModelId& id) const;

template<typename ModelIdT>
void ModelContainer::set(const ModelIdT& id, ModelPtr<ModelIdT>&& model)
{
    auto& modelMap = std::get<ModelMap<ModelIdT>>(pImpl->models);
    auto node = std::pair{id,std::move(model)};
    modelMap.emplace(std::move(node));
}

template void ModelContainer::set(const InterestRateCurveId& id, ModelPtr<InterestRateCurveId>&& model);
template void ModelContainer::set(const HazardRateCurveId& id, ModelPtr<HazardRateCurveId>&& model);
template void ModelContainer::set(const S3ModelId& id, ModelPtr<S3ModelId>&& model);
