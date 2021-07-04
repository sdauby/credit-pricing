#include "InstrumentElaborator.hpp"
#include "Core/Currency.hpp"
#include "Core/Data.hpp"
#include "Instruments/Cds.hpp"
#include "Instruments/FixedCouponBond.hpp"
#include "Instruments/FloatingCouponBond.hpp"
#include "Instruments/IRSwap.hpp"

InstrumentElaborator::InstrumentElaborator(const InstrumentFactory& makeInstrument,
                                           const InstrumentId& id) : 
                                           makeInstrument_(makeInstrument),
                                           id_(id)
{}

std::vector<VariantId> InstrumentElaborator::getRequestBatch(const Container&)
{
    return {};
}

std::unique_ptr<Instrument> InstrumentElaborator::make(const Container& container) 
{
    return makeInstrument_(id_);
}
