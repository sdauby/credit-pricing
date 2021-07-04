#pragma once

enum class PricerKind { General, IR, S3 };

constexpr const char* name(PricerKind kind)
{
    switch (kind) {
        case PricerKind::General : return "General";
        case PricerKind::IR      : return "IR";
        case PricerKind::S3      : return "S3";
    }
}
