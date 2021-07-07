#pragma once

#include "Core/Date.hpp"

#include <vector>

// Indices of the elements of t in the term structure T.
// All elements of t must belong to T.
std::vector<int> indices(const std::vector<Date>& t, const std::vector<Date>& T);

// Insert T0 at the front of a tenor structure if it is not already there.
void includeT0(std::vector<Date>& t);
