#pragma once

#include "Instruments/Instrument.hpp"

#include <memory>

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

// usage: make<instrumentKind>(instrumentData)
template<typename InstrumentType>
constexpr auto make = [] (auto&&... xs) {
    using DataType = typename InstrumentType::DataType;
    return std::make_unique<InstrumentType>(DataType{std::forward<decltype(xs)>(xs)...});
};
