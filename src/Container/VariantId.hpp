#pragma once

#include "Container/IdTypes.hpp"

#include <string>

using VariantId = ApplyToIdTypes<std::variant>;

std::string to_string(const VariantId& id);
