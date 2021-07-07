#pragma once

#include "Instruments/FloatingCouponBond.hpp"

struct S3ModelId;
class S3Model;

S3ModelId modelId(const FloatingCouponBond& bond);
double pv(const FloatingCouponBond& bond, const S3Model& model);

