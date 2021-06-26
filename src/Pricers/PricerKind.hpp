#pragma once

enum class PricerKind { IR, S3 };

constexpr const char* name(PricerKind kind)
{
    switch (kind) {
        case PricerKind::IR: return "IR";
        case PricerKind::S3: return "S3";
    }
}
