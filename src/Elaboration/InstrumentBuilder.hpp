#pragma once

#include "Builder.hpp"

using InstrumentFactory = std::function<std::unique_ptr<Instrument>(const InstrumentId&)>;

class InstrumentBuilder : public Builder<InstrumentId> {
public:
    InstrumentBuilder(const InstrumentFactory& makeInstrument,
                         const InstrumentId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<Instrument> getObject(const Container& container) override;

    const InstrumentFactory& makeInstrument_;
    InstrumentId id_;
};
