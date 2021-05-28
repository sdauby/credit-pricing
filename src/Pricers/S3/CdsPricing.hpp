#pragma once
#include "Instruments/Cds.hpp"

struct S3ModelId;
class S3Model;
    
S3ModelId modelId(const Cds& cds);
double pv(const Cds& cds, const S3Model& model);
