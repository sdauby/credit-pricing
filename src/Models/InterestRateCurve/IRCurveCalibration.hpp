#pragma once

#include "Instruments/IRSwap.hpp"
#include "Models/InterestRateCurve/InterestRateCurve.hpp"

#include <memory>
#include <vector>

class IRCurveCalibration {
public:
    using InstrumentVector = std::vector<std::unique_ptr<IRSwap>>;
    using PriceVector = std::vector<double>;

    explicit IRCurveCalibration(InstrumentVector&& instruments);
    
    std::unique_ptr<InterestRateCurve> calibrate(const PriceVector& prices) const;
    const InstrumentVector& instruments() const;
private:
    InstrumentVector instruments_;
    std::vector<Date> T_;
};
