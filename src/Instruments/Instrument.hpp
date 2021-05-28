#pragma once

enum class InstrumentKind;

class Instrument {
public:
    virtual ~Instrument() = default;
    Instrument() = default;
    Instrument(const Instrument&) = delete;
    Instrument& operator=(const Instrument&) = delete;

    virtual InstrumentKind kind() const = 0;
};
