cmake_minimum_required(VERSION 3.20)

project(credit-pricing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(.)

add_executable(main
Core/Date.cpp
Core/Data.cpp
Core/Solve.cpp
Core/Tabulator.cpp
Models/HazardRateCurve/HazardRateCurve.cpp
Models/InterestRateCurve/InterestRateCurve.cpp
Models/InterestRateCurve/IRSwapAnalytics.cpp
Models/InterestRateCurve/IRCurveCalibration.cpp
Models/S3/S3Model.cpp
Models/S3/Utilities.cpp
ModelFactory/ModelFactory.cpp
Pricers/IR/IRPricer.cpp
Pricers/S3/CdsPricing.cpp
Pricers/S3/FixedCouponBondPricing.cpp
Pricers/S3/FloatingCouponBondPricing.cpp
Pricers/S3/S3Pricer.cpp
Container/Container.cpp
Metrics/Metric.cpp
Metrics/PVImpl.cpp
Metrics/IRDeltaImpl.cpp
Metrics/Result.cpp
PricingEngine/PricingConfiguration.cpp
PricingEngine/PricingEngine.cpp
main.cpp
)