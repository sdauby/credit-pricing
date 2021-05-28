#pragma once

enum class Currency {
    EUR,
    GBP,
    JPY,
    USD,
};

constexpr const char* name(Currency ccy) 
{
    switch (ccy) {
        using Ccy = Currency;
        case Ccy::EUR: return "EUR";
        case Ccy::GBP: return "GBP";
        case Ccy::JPY: return "JPY";
        case Ccy::USD: return "USD";
    }
}