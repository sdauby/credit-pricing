#pragma once

#include "Container/Id.hpp"

#include <string>
#include <variant>

using VariantId = WithAllIdTypes<std::variant>;

std::string to_string(const VariantId& id);
