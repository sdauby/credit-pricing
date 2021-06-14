#pragma once
#include "Instruments/Portfolio.hpp"
#include "Pricers/Pricer.hpp"

class PricingConfiguration;
class ModelFactory;

namespace PricingEngine {
    PvResult price(const InstrumentMap& instruments, 
                   const PricingConfiguration& config,
                   const ModelFactory& modelFactory);
}
