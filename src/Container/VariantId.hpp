#pragma once

#include "Container/IdTypes.hpp"

#include <string>

template<typename... Ts> std::variant<Ts...> auxVariantId(typelist<Ts...>);
using VariantId = decltype(auxVariantId(IdTypes{}));

std::string to_string(const VariantId& id);
