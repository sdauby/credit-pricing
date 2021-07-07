#include "Core/Date.hpp"

double dayCountFactor(Date t1, Date t2) 
{
    using namespace std::chrono;
    const auto years = t2-t1;
    return years.count();
}