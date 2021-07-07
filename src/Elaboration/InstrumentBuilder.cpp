#include "Elaboration/InstrumentBuilder.hpp"

#include "Instruments/Instrument.hpp"

InstrumentBuilder::InstrumentBuilder(const InstrumentFactory& makeInstrument,
                                           const InstrumentId& id) : 
                                           makeInstrument_(makeInstrument),
                                           id_(id)
{}

std::vector<VariantId> InstrumentBuilder::getRequestBatch(const Container&)
{
    return {};
}

std::unique_ptr<Instrument> InstrumentBuilder::getObject(const Container&) 
{
    return makeInstrument_(id_);
}
