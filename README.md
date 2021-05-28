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
Here are a few unimplemeted features, out of a great many, with architectural significance:
* Greeks and market scenarios;
* Live market data updates, live portfolio updates;
* Distributed computation.
The implemented design is minimal for the implemented feature set (i.e. it does not contain
any capability which is not used), so it is easily adaptable to such requirements.

2. Overview

2.1 The main program

This is the output of the main program.

```
sdauby@MacBook-Pro-de-Sebastien build % ./main 
Using the S3 pricer:
PositionId InstrumentId InstrumentKind     PVCurrency PV          
---------- ------------ ------------------ ---------- ----------- 
0          0            Cds                EUR        -22872.2    
1          0            Cds                EUR        22872.2     
2          1            Cds                USD        -53273      
3          2            FixedCouponBond    USD        957426      
4          3            FixedCouponBond    EUR        931474      
5          4            FloatingCouponBond USD        999432      
6          5            FloatingCouponBond USD        816645      

Using the IR pricer:
PositionId InstrumentId InstrumentKind     PVCurrency PV          
---------- ------------ ------------------ ---------- ----------- 
0          0            Cds                NA         NA          
1          0            Cds                NA         NA          
2          1            Cds                NA         NA          
3          2            FixedCouponBond    USD        999852      
4          3            FixedCouponBond    EUR        959322      
5          4            FloatingCouponBond USD        1.02941e+06 
6          5            FloatingCouponBond USD        884690      
```

We can see:
* The program prices a portfolio successively with the S3 (i.e. credit) pricer and the IR pricer.
* It is possible for several positions to share the same instrument.
* Some instruments are supported by both pricers, some by one pricer only.

2.2 Code structure




4. Implementation notes

