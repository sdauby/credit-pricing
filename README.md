# credit-pricing

## 1. Introduction

This project is the prototype of a pricing engine for financial products.
The focus is on the component architecture, so the business logic components are extremely simplified.

The business logic components are:
* 3 kinds of instruments (fixed coupon bond, floating coupon bond, credit default swap);
* 3 market models (interest rate curve, hazard rate curve, S3 model (S3 stands for Schönbucher chapter 3));
* 2 pricers: IR for interest-rate pricing, S3 for credit pricing.

The notable architectural components are:
* The Instrument abstraction;
* Portfolio, a container for instruments and positions on the instruments;
* The Model abstraction;
* ModelContainer, a container for models;
* The Pricer abstraction: a Pricer joins Instruments with Models to produce PVs;
* PricingConfiguration, an encapsulation of the Pricer selection strategy;
* The PricingEngine, which produces the PVs of a Portfolio given a PricingConfiguration.

Even in the architectural dimension, this is very far from a complete prototype.
Here are a few unimplemeted features, out of a great many, with architectural significance:
* Model calibration;
* Greeks and market scenarios;
* Live market data updates, live portfolio updates;
* Distributed computation.

The implemented design is minimal for the implemented feature set (i.e. it does not contain
any capability which is not used). 
It is easily extendable.

## 2. Overview

### 2.1 The main program

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

We can observe that:
* The program prices a portfolio successively with the S3 (i.e. credit) pricer and the IR pricer.
* It is possible for several positions to share the same instrument.
* Some instruments are supported by both pricers, some by one pricer only.

## 2.2 Code structure

The below diagram presents the components of the system ordered by their dependencies.

![Components](https://github.com/sdauby/credit-pricing/blob/main/svg/components.svg "Components")

Each component is implemented in the corresponding folder.
I will now describe the components from the bottom up.

## 3. Components

## 3.1 Core

This component defines basic financial types: `Date`, `Currency`, `Issuer` and associated functions.

`Date` is an alias of `std::chrono::year`: we cannot represent dates at a lower granularity than the year.
This extreme simplification obviates day count factor complications.

I have also placed here the `Data` namespace, which provides a few hard-coded market values.

## 3.2 Instruments

#### 3.2.1 Instrument

The `InstrumentKind` enum class assigns an enumerator to every instrument type (`FixedCouponBond`, `FloatingCouponBond`, `Cds`).

The `Instrument` interface is intended to be derived by concrete instrument types.
Its raison d'être is to enable the collection of heterogeneous instruments in a single container.
The only useful thing one can do with an `Instrument` object is retrieve its `InstrumentKind`.
Having the `InstrumentKind`, one can `static_cast` to the concrete type and access its data.

`InstrumentImpl` is a class template parameterized by an `InstrumentKind` value and a `DataT` type.
The `DataT` type parameter is intended to be a concrete type representing instrument data.
`InstrumentImpl` instances derive from the `Instrument` interface, and have a `DataT` data member, to
which they give read-only access via a `data()` non-virtual member function.
Finally, they provide a constructor which moves a `DataT` argument into the `DataT` data member.

The `FixedCouponBond`, `FloatingCouponBond` and `Cds` concrete classes are derived from `Instrument`,
instantiated from the `InstrumentImpl` template, with respectively the `FixedCouponBondData`,
`FloatingCouponBondData` and `CdsData` structs as `DataT` parameter.
For example, `FixedCouponBondData` represents the data of a fixed coupon bond:
```c++
struct FixedCouponBondData {
    Currency ccy = {};
    Issuer issuer;
    std::vector<Date> t; // period dates
    double c = {}; // coupon rate
};
```

#### 3.2.2 Portfolio

`InstrumentId` and `PositionId` are `unsigned` aliases. (Improvement: make them proper types in the interest of type safety.)
They identify `Instrument`s and `Position`s in the context of a `Portfolio`.

A `Position` represents a quantity of an instrument identified by its `InstrumentId`.
A `Portfolio` consists of a collection of `Instrument`s and a collection of `Position`s.
The concept of position is useful to represent the situation where several positions refer to the same instrument,
as may happen in particular for fungible, standardized instruments.
For example, when pricing a portfolio containing several positions on the same instrument, we need to price
the common instrument only once.

The `Instrument`s contained in a `Portfolio` are held through `unique_ptr<Instrument>`s, so:
* `Portfolio` has no direct knowledge of the concrete types of the `Instrument`s;
* `Portfolio` has sole ownership of the `Instrument`s.

## 3.3 Models

### 3.3.1 The concept of model

I use the term *model* in a very inclusive sense.
Any object whose values are derived from instrument market prices, and from which instrument model prices can be derived,
is a model.
This applies e.g. to Libor Market Models, to SABR models, to discount curves.
At the lowest end of the spectrum of model complexity lies the degenerate case of raw market prices.
(Improvement: specify the concept further with a bit of abstract, mathematical-style formalism.)

### 3.3.2 Implementation

Within the Models component, there is no C++-level construct shared by every model. Each model stands alone.

`InterestRateCurve` is extremely simplified. 
Its data consist in just one `double` representing a flat continuously compounded rate. 

`HazardRateCurve` is also extremely simplified, with a single double parameter.
It represents the default risk of a certain issuer in a certain currency.
It has a `hazardRate()` member function, which provides forward hazard rates.
(Improvement: I think that the implementation of this class is not mathematically correct.)

`S3Model` is an implementation of the "building blocks for credit derivative pricing" of Philipp J. Schönbucher's
*Credit Derivatives Pricing Models* book, chapter 3.
Its constructor moves a tenor structure of forward rates and hazard rates, as well as a recovery rate, into its member data.
Its member functions (`T`, `delta`, `B`, `Bbar`, `e`, `pi`, `F`) provide the building blocks in the notations of
the book.

# 3.4 ModelContainer

This component implements a container mapping model identifiers to models.

# 3.4.1 Model identifiers

The `InterestRateCurveId`, `HazardRateCurveId` and `S3ModelId` structs each contain a `ModelType` type alias, equal
respectively to `InterestRateCurve`, `HazardRateCurve` and `S3Model`.
In that way, we establish a `ModelIdT` to `ModelT` correspondance for every model type `ModelT`.
(In the rest of this document, *the `ModelIdT` types* will mean `InterestRateCurveId`, `HazardRateCurveId` and `S3ModelId`.)

Each `ModelIdT` type contains data which specify the segment of the financial markets which is
modelled and abstracted by a model (cf 3.3.1).
For example, `HazardRateCurveId` contains an issuer and a currency. The value
`HazardRateCurveId{Currency::USD,Issuers::JPM}` identifies the hazard rate curve representing the default risk of issuer
JP Morgan in USD.

In summary, a value of a `ModelIdT` type uniquely identifies a specific 
segment of the financial markets (e.g. the USD rates; the USD JPM default risk; the overall USD JPM credit)
and a model type (via the `ModelT` type alias) adequate for that segment.

# 3.4.2 `ModelId`

`ModelId` is a `std::variant` whose alternatives are `InterestRateCurveId`, `HazardRateCurveId` and `S3ModelId`.
Therefore, it is an identifier for a model of either of the corresponding types.

It is useful, for example, to enable the collection of heterogeneous `ModelIdT` values in a single container.
In section 4, I compare the `ModelT` / `ModelIdT` type relationship on the one hand, and the `Instrument` / `InstrumentImpl`
instance relationship on the other hand.

(Improvement: `ModelId` has no role in ModelContainer so it would rather belong with Pricers.)

# 3.4.3 ModelContainer

`ModelContainer` contains objects of the `ModelT` types, keyed by values of the `ModelIdT` types.
It uses `std::unique_ptr` so it assumes sole ownership of the model objects it contains.

It provides member function templates `get()` and `set()`, parameterized by the `ModelIdT` type.

Interestingly, the `InterestRateCurveId`, `HazardRateCurveId` and `S3ModelId` types, as well as
the `InterestRateCurve`, `HazardRateCurve` and `S3Model` types, are completely hidden from `ModelContainer.hpp`:
* They do not occur in the data members because we use the pImpl idiom.
* They do not occur in the `get()` and `set()` member functions because they are templates which abstract those types via the template type parameter.
So, we can add concrete model types to `ModelContainer` without recompiling any existing `ModelContainer` client.

(Improvement: it may be possible to further factorize `ModelContainer.cpp` with a variadic template taking
all the `ModelIdT` types.)

## 4. Implementation notes
C++20
Level of sophistication, abstraction
Templates, concepts
Smart pointers, references, raw pointers
References to Meyers, Stroustrup, Alexandrescu, C++ Core Guidelines
Why Instruments need static_cast
Variant (ModelId) vs interface (Instrument)
Hash map vs ordered map

variant Stroutstrup Tour 13.5.1


