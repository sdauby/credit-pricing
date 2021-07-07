#pragma once

#include "Container/VariantId.hpp"

#include <map>
#include <vector>

using IdDag = std::map<VariantId,std::vector<VariantId>>;

struct IdDagAux {
    IdDag direct;
    IdDag inverse;
    std::vector<VariantId> roots;
    std::vector<VariantId> leaves;
};

IdDagAux makeAux(IdDag direct);
