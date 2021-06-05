#pragma once
#include <functional>

using Function = std::function<double(double)>;
double solve(const Function& f, double x, double y);