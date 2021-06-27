#pragma once
#include "VariantId.hpp"

struct ContainerDag {
    std::map<VariantId,std::vector<VariantId>> precedents;
    std::map<VariantId,std::vector<VariantId>> dependents;
    std::vector<VariantId> roots; // nodes with no precedents
};
