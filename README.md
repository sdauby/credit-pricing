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

A `Position` represents a quantity of an instrument.
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

Within the Models component, there is no C++-level construct shared by every model.

`InterestRateCurve` is extremely simplified. 
Its data consist in just one `double` representing a flat continuously compounded rate. 

`HazardRateCurve` is also extremely simplified, with a single double parameter.
It represents the default risk of a certain issuer in a certain currency.
It has a `hazardRate()` member function, which provides forward hazard rates.
(Improvement: I think that the implementation of this class is not mathematically correct.)

`S3Model` is an implementation of the "building blocks for credit derivative pricing" of Philipp J. Schönbucher's
*Credit Derivatives Pricing Models*, chapter 3.
Its constructor moves a tenor structure of forward rates and hazard rates, as well as a recovery rate, into its member data.
Its member functions (`T`, `delta`, `B`, `Bbar`, `e`, `pi`, `F`) provide the building blocks in the notations of
the book.

## 3.4 ModelContainer

This component implements a container mapping model identifiers to models.

### 3.4.1 Model identifiers

The `InterestRateCurveId`, `HazardRateCurveId` and `S3ModelId` structs each contain a `ModelType` type alias, equal
respectively to `InterestRateCurve`, `HazardRateCurve` and `S3Model`.
In that way, we establish a `ModelIdT` to `ModelT` correspondance for every model type `ModelT`.
(In the rest of this document, *the `ModelIdT` types* will mean `InterestRateCurveId`, `HazardRateCurveId` and `S3ModelId`.)

A `ModelIdT` value contains data which specify the segment of the financial markets which is
modelled and abstracted by a model (cf 3.3.1).
For example, `HazardRateCurveId` contains an issuer and a currency. The value
`HazardRateCurveId{Currency::USD,Issuers::JPM}` identifies the hazard rate curve representing the default risk of issuer
JP Morgan in USD.

### 3.4.2 `ModelId`

`ModelId` is a `std::variant` whose alternatives are `InterestRateCurveId`, `HazardRateCurveId` and `S3ModelId`.
Therefore, it is an identifier for a model of any of the corresponding types.

It is useful, for example, to enable the collection of heterogeneous `ModelIdT` values in a single container.
The `ModelT` / `ModelIdT` type relationship is analogous to the `Instrument` / `InstrumentImpl`
instance relationship.
Yet, they are implemented differently.
The reason is that `std::variant`, being a concrete type, is preferrable to a class hierarchy
(cf C++ Core Guidelines C.10).
The reason for not using a `std::variant` for instruments is that, unlike model identifiers, some instrument types
are likely to have many data members. This makes a `std::variant` potentially memory-inefficient.
Having said that, both choices can be revisited.

instances are expected to be small, fast to move objects

(Improvement: `ModelId` has no role in ModelContainer so it would rather belong with Pricers.)

### 3.4.3 ModelContainer

`ModelContainer` contains objects of the `ModelT` types, keyed by values of the `ModelIdT` types.
It uses `std::unique_ptr` so it assumes sole ownership of the model objects it contains.

It provides member function templates `get()` and `set()`, parameterized by the `ModelIdT` type.

Interestingly, the `InterestRateCurveId`, `HazardRateCurveId` and `S3ModelId` types, as well as
the `InterestRateCurve`, `HazardRateCurve` and `S3Model` types, are completely hidden from `ModelContainer.hpp`:
* They do not occur in the data members because we use the pImpl idiom.
* They do not occur in the `get()` and `set()` member functions because they are templates which abstract those types via the template type parameter.

So, we can add concrete model types to `ModelContainer` without recompiling any existing `ModelContainer` client.

The `get()` member function template returns a raw pointer. 
This is standard for non-owning references which may be null. (Cf C++ Core Guidelines F.60.)

(Improvement: it may be possible to further factorize `ModelContainer.cpp` with a variadic template taking
all the `ModelIdT` types.)

## 3.5 Pricers

### 3.5.1 Pricer

`Pricer` is an interface for objects which manage the pricing of a collection of instruments.
The `requiredModels()` pure virtual function returns the identifiers of the model objects required to price the instruments.
The `pvs()` pure virtual function takes a `ModelContainer` reference and returns a map from `InstrumentId` to (pv,currency) pairs.
The `ModelContainer` is expected to contain models for all the identifiers returned by `requiredModels()`.

### 3.5.2 S3Pricer

`S3Pricer` is derived from `Pricer`. 
It is able to price fixed and floating coupon bonds and credit default swaps.
It uses the S3 model, i.e. the Schönbucher's "building blocks", to derive instrument prices.

`S3Pricer` composes `S3UnitPricer`s, each of which is responsible for a single instrument.
They are created by the `makeS3UnitPricer()` factory function, where we see the `kind()` and `static_cast` pattern
mentioned in 3.2.1.
I prefer this pattern to having a `makeS3UnitPricer()` virtual function on the `Instrument` interface because in the
high-level architecture (see 2.2) the Instruments belongs to a lower layer than the Pricers layer.
For example, we may want to build a PaymentProcessing component on top of Instruments, and that component would
have nothing to do with Pricers.
So, it would be bad architecture to encumber `Instrument` with pricer-related member functions.
The approach I have chosen is reminiscent of the Visitor design pattern.

The `aggregateTenorStructures()` function analyses all the `S3ModelId`s required by the `S3UnitPricer`s.
If two requirements differ only by their required tenor structure, those tenor structures are merged and
a single model having the merged tenor structure is created.
For example, this ensures that a 3Y CDS on JPM in USD and a 5Y CDS on JPM in USD use one S3 model rather
than two.
Although this example is a little contrived because of the extreme simplicity of the S3 model, it illustrates
that grouping instruments priced with the same method (S3, IR) enables performance optimisations, something
that would not be possible if the `Pricer` interface were for single instruments rather than collections of
instruments.

### 3.5.3 IRPricer
`IRPricer`is derived from `Pricer`. It is able to price fixed and floating bonds only.
It uses the InterestRateCurve model.

`IRPricer` is architecturally similar to `S3Pricer`.
(Improvement: factorise the architectural commonalities of `IRPricer` and `S3Pricer`,
e.g. define a `UnitPricer` interface and define a `PricerImpl` class template to generate
`Pricer` derived classes aggregating `UnitPricer`s.)

## 3.6 PricingEngine

`PricingEngine` is a namespace with a single function
```c++
PvResult price(const InstrumentMap& instruments, const PricingConfiguration& config);
```
which returns instrument prices.

The `PricingConfiguration` class encapsulates the strategy for the selection of the pricer kind (IR or S3) to be applied to a given instrument.
In the example program, two strategies are applied successively:
* S3 for all instruments;
* IR for instruments which support IR, nothing for the other instruments.

`price()` calls `makePricers()`, which groups instruments by pricer kind and instantiates 
an `IRPricer` and a `S3Pricer` with the ids of the instruments each must price.

It instantiates a `ModelContainer` and calls `ModelFactory::populate()` to
populate it with all the models required by the pricers.
This function uses an interesting idiom (`std::visit()` and `overloaded` lambdas,
cf. Stroustrup, *A Tour of C++*, 13.5.1) to dispatch the model creation
to the adequate factory function based on the alternative contained in the
`ModelId` variant.

`ModelFactory` is unfinished work. It is meant to evolve into a mechanism to
manage the dependencies between models. This can be useful for efficient recalculations
on market data changes, e.g. for greeks, scenarios and live market data updates.

## 4. General comments

### 4.1 Object ownership model

All the objects allocated on the free-store are placed in `std::unique_ptr`s and moved into
high-level containers retaining sole ownership: `Portfolio`, `ModelContainer`.

Read-only access to these objects is provided by:
* the containers, in the form of constant reference return values
(exceptionally in the form of constant pointer return values
to express that a null value is possible),
* constant reference arguments of functions,
* constant references stored in classes, initialized in the class constructors from constant reference arguments.

In all those cases, the references must not outlive the owning containers.
This is very easily achieved, without recourse to `shared_ptr`s.
This approach is consistent with C++ Core Guidelines R.21, F.7, R.37.

### 4.2 Maps

All the maps are ordered maps.
They should be hashed maps for efficiency.

