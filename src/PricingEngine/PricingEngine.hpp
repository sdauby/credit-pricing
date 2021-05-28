#pragma once

struct Portfolio;
class PricingConfiguration;

namespace PricingEngine {
    void price(const Portfolio& p, const PricingConfiguration& config);
}
