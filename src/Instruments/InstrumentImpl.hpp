#pragma once
#include "Instrument.hpp"

enum class InstrumentKind;

template<InstrumentKind instrumentK, typename DataT>
class InstrumentImpl : public Instrument {
public:
    using DataType = DataT;
    static constexpr InstrumentKind instrumentKind = instrumentK; 
    
    InstrumentImpl(DataType&& data) : data_(std::move(data)) {}
    
    InstrumentKind kind() const override { return instrumentK; }
    
    const DataType& data() const { return data_; }

private:
    DataType data_;
};
