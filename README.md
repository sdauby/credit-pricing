# credit-pricing

1. Introduction

This project is the prototype of a pricing engine for financial products.
The focus is on the component architecture so the business logic components are extremely simplified.

The business logic components are:
* 3 kinds of instruments (fixed coupon bond, floating coupon bond, credit default swap);
* 3 market models (interest rate curve, hazard rate curve, S3 model (S3 stands for Schoenbucher chapter 3));
* 2 pricers: IR for interest-rate pricing, S3 for credit pricing.

The notable architectural components are:
* The Instrument abstraction;
* Portfolio, a container for instruments and positions on the instruments;
* The Model abstraction;
* ModelContainer, a container for models;
* The Pricer abstraction: a Pricer joins Instruments with Models to produce PVs;
* PricingConfiguration, an encapsulation of the Pricer selection strategy;
* The PricingEngine, which produces the PVs of a Portfolio given a PricingConfiguration.

Even in the architectural dimension, this is far from a complete prototype.
Here are a few further developments which would make sense:
* Greeks and market scenarios;
* Live market data updates, live portfolio updates;
* Distributed computation.

2. Overview
3. Implementation notes

