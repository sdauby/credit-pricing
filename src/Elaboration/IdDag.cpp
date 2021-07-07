#include "Elaboration/IdDag.hpp"

IdDagAux makeAux(IdDag direct) 
{
    IdDag inverse;
    std::vector<VariantId> leaves;

    for (const auto& [source, destinations] : direct) {
        inverse[source];
        if (destinations.empty())
            leaves.push_back(source);
        else {
            for (const VariantId& destination : destinations)
                inverse[destination].push_back(source);
        }
    }

    std::vector<VariantId> roots;
    for (const auto& [destination, sources] : inverse) {
        if (sources.empty())
            roots.push_back(destination);
    }

    return { 
        .direct = std::move(direct),
        .inverse = std::move(inverse),
        .leaves = std::move(leaves),
        .roots = std::move(roots),
     };
}
