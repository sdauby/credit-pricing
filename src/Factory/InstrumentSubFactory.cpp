#include "InstrumentSubFactory.hpp"

std::vector<VariantId> 
InstrumentSubFactory::getPrecedents(const InstrumentId& instrumentId, const Container& container) const
{
    return {};
}
    
std::unique_ptr<Instrument>
InstrumentSubFactory::make(const InstrumentId& instrumentId, const Container& container) const
{
    return {};
}
