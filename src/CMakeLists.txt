cmake_minimum_required(VERSION 3.20)

project(credit-pricing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -pedantic -Werror)

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
Pricers/General/GeneralPricer.cpp
Pricers/IR/IRPricer.cpp
Pricers/S3/CdsPricing.cpp
Pricers/S3/FixedCouponBondPricing.cpp
Pricers/S3/FloatingCouponBondPricing.cpp
Pricers/S3/S3Pricer.cpp
Pricers/PricingConfiguration.cpp
Container/VariantId.cpp
Elaboration/Elaboration.cpp
Elaboration/PricerBuilder.cpp
Elaboration/InstrumentBuilder.cpp
Elaboration/HazardRateCurveBuilder.cpp
Elaboration/InterestRateCurveBuilder.cpp
Elaboration/S3ModelBuilder.cpp
Elaboration/IdDag.cpp
Metrics/PV.cpp
Metrics/IRDelta.cpp
Metrics/Result.cpp
Metrics/UpdatePropagation.cpp
PricingEngine/PricingEngine.cpp
main.cpp
)