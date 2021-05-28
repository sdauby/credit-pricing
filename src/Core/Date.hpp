#pragma once
#include <chrono>

using Date = std::chrono::year;

double dayCountFactor(Date t1, Date t2);