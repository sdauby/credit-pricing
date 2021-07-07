#include "Container/VariantId.hpp"

std::string to_string(const VariantId& id) 
{
    return std::visit( [] (const auto& id) { return to_string_(id); }, id );
}
