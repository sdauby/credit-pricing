#include "InstrumentBuilder.hpp"
#include "Core/Currency.hpp"
#include "Core/Data.hpp"
#include "Instruments/Cds.hpp"
#include "Instruments/FixedCouponBond.hpp"
#include "Instruments/FloatingCouponBond.hpp"
#include "Instruments/IRSwap.hpp"

InstrumentBuilder::InstrumentBuilder(const InstrumentFactory& makeInstrument,
                                           const InstrumentId& id) : 
                                           makeInstrument_(makeInstrument),
                                           id_(id)
{}

std::vector<VariantId> InstrumentBuilder::getRequestBatch(const Container&)
{
    return {};
}

std::unique_ptr<Instrument> InstrumentBuilder::getObject(const Container& container) 
{
    return makeInstrument_(id_);
}
