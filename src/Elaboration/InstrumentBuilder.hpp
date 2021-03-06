#pragma once

#include "Elaboration/Builder.hpp"

using InstrumentFactory = std::function< std::unique_ptr<Instrument> (const InstrumentId&) >;

class InstrumentBuilder : public Builder<InstrumentId> {
public:
    InstrumentBuilder(const InstrumentFactory& makeInstrument,
                      const InstrumentId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<Instrument> getObject(const Container&) override;

    const InstrumentFactory& makeInstrument_;
    InstrumentId id_;
};
