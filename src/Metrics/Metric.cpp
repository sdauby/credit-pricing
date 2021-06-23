#include "Metric.hpp"
#include "PVImpl.hpp"
#include "IRDeltaImpl.hpp"

std::unique_ptr<MetricImpl> makeMetricImpl(Metric metric) 
{
    switch (metric) {
        case Metric::PV: return std::make_unique<PVImpl>();
        case Metric::IRDelta : return std::make_unique<IRDeltaImpl>();
    }
}
