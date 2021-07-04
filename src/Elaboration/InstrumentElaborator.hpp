#pragma once

#include "Elaborator.hpp"

using InstrumentFactory = std::function<std::unique_ptr<Instrument>(const InstrumentId&)>;

class InstrumentElaborator : public Elaborator<InstrumentId> {
public:
    InstrumentElaborator(const InstrumentFactory& makeInstrument,
                         const InstrumentId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<Instrument> make(const Container& container) override;

    const InstrumentFactory& makeInstrument_;
    InstrumentId id_;
};
