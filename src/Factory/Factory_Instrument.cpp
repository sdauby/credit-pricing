#include "Factory.hpp"

template<>
std::vector<VariantId> Factory::getPrecedents(const InstrumentId&, const Container&) const
{ 
    return {}; 
}

template<>
std::unique_ptr<Object<InstrumentId>> Factory::make(const InstrumentId&, const Container&) const
{
    return {};
}
