#pragma once

#include "Instruments/FixedCouponBond.hpp"

struct S3ModelId;
class S3Model;
    
S3ModelId modelId(const FixedCouponBond& bond);
double pv(const FixedCouponBond& bond, const S3Model& model);
