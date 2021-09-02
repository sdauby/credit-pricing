# credit-pricing

## 1. Introduction

This project is the prototype of a pricing engine for financial products.
The focus is on the component architecture, so the business logic components are extremely simplified.

The business logic components are:
* 4 kinds of instruments (fixed coupon bond, floating coupon bond, credit default swap, interest rate swap);
* 3 market models (interest rate curve, hazard rate curve, S3 model (S3 stands for Schönbucher chapter 3));
* 2 pricers: IR for interest-rate pricing, S3 for credit pricing.

Here are the notable architectural components.
* The Instrument abstraction.
* The Container is a structure containing objects such as instruments, pricers, models, market data. It facilitates
the management of multiple states of those objects, which in turn provides a foundation for scenarios and greeks.
* Elaboration is a configurable process for assembling in a Container, from various data sources, all the
data required for a specific computation.
* The Models module provides market models, the building blocks of instrument pricing.
* The Pricer abstraction. A Pricer uses Models to produce Instrument PVs from the contents of a Container.
* The Metrics module combines the Pricer abstraction with Container states to implement greeks.
* PricingConfiguration encapsulates the Pricer selection strategy.
* The PricingEngine produces Instrument PVs and greeks given a PricingConfiguration.

Even in the architectural dimension, this is very far from a complete prototype.
Here are a few unimplemeted features, out of a great many, with architectural significance:
* Model calibration;
* Live market data updates, live portfolio updates;
* Distributed computation.

## 2. Overview

### 2.1 The main program

This is the output of the main program.

```
sdauby@MacBook-Pro-de-Sebastien build % ./main 
Prefer the S3 pricer:
                                                                              IRDelta     IRDelta    
                                                  PV            PV            EUR         USD        
PositionId InstrumentId InstrumentKind     Pricer EUR           USD           EUR         USD        
---------- ------------ ------------------ ------ ------------- ------------- ----------- -----------
0          0            Cds                S3     -22767.330472               6.694546               
1          0            Cds                S3     22767.330472                -6.694546              
2          1            Cds                S3                   -50680.643370             20.137811  
3          2            FixedCouponBond    S3                   929781.181714             -271.850462
4          3            FixedCouponBond    S3     932414.382402               -545.311562            
5          4            FloatingCouponBond S3                   998601.028045             -5.435533  
6          5            FloatingCouponBond S3                   819476.507922             40.892316  
7          6            IRSwap             IR                   -0.000000                 -480.772870

Prefer the IR pricer:
                                                                               IRDelta     IRDelta    
                                                  PV            PV             EUR         USD        
PositionId InstrumentId InstrumentKind     Pricer EUR           USD            EUR         USD        
---------- ------------ ------------------ ------ ------------- -------------- ----------- -----------
0          0            Cds                S3     -22767.330472                6.694546               
1          0            Cds                S3     22767.330472                 -6.694546              
2          1            Cds                S3                   -50680.643370              20.137812  
3          2            FixedCouponBond    IR                   971161.167274              -288.426397
4          3            FixedCouponBond    IR     960444.054515                -568.875668            
5          4            FloatingCouponBond IR                   1028838.832726             -5.729697  
6          5            FloatingCouponBond IR                   889080.576422              43.782190  
7          6            IRSwap             IR                   -0.000000                  -480.772870
```

We can observe that:
* The program produces PVs and IR risk for a portfolio successively with the S3 (i.e. credit) pricer and the IR pricer.
* It is possible for several positions to share the same instrument.
* Some instruments are supported by both pricers, some by one pricer only.

## 2.2 Code structure

The below diagram presents the components and a simplified view of their dependency relationships.

![Components](https://github.com/sdauby/credit-pricing/blob/main/svg/components.svg "Components")

Each component is implemented in the corresponding folder.
I will now describe the components from the bottom up.

## 3. Components

## 3.1 Core

This component defines basic financial types: `Date`, `Currency`, `Issuer` and associated functions.

`Date` is an alias of `std::chrono::year`: we cannot represent dates at a lower granularity than the year.
This extreme simplification obviates day count factor complications.

I have also placed here the `Data` namespace, which provides a few hard-coded market values.

There is also the `solve()` function, a very primitive root solver.

## 3.2 Instruments

The `InstrumentKind` enum class assigns an enumerator to every instrument type (`FixedCouponBond`, `FloatingCouponBond`, `Cds`, `IRSwap`).

The `Instrument` interface is derived by concrete instrument types.
Its raison d'être is to enable the collection of heterogeneous instruments in a single container.
The only useful thing one can do with an `Instrument` object is retrieve its `InstrumentKind`.
Having the `InstrumentKind`, one can `static_cast` to the concrete type and access its data.

`InstrumentImpl` is a class template parameterized by an `InstrumentKind` value and a `DataT` type.
The `DataT` type parameter is a concrete type representing instrument data.
`InstrumentImpl` instances derive from the `Instrument` interface, and have a `DataT` data member, to
which they give read-only access via a `data()` non-virtual member function.
Finally, they provide a constructor which moves a `DataT` argument into the `DataT` data member.

The `FixedCouponBond`, `FloatingCouponBond`, `Cds` and `IRSwap` concrete classes are derived from `Instrument`,
instantiated from the `InstrumentImpl` template, with respectively the `FixedCouponBondData`,
`FloatingCouponBondData`, `CdsData` and `IRSwapData` structs as `DataT` parameter.
For example, `FixedCouponBondData` represents the data of a fixed coupon bond:
```c++
struct FixedCouponBondData {
    Currency ccy = {};
    Issuer issuer;
    std::vector<Date> t; // period dates
    double c = {}; // coupon rate
};
```

## 3.3 Models

### 3.3.1 The concept of model

I use the term *model* in a very inclusive sense.
Any object whose values are derived from instrument market prices, and from which instrument model prices can be derived,
is a model.
This applies e.g. to Libor Market Models, to SABR models, to discount curves.
At the lowest end of the spectrum of model complexity lies the degenerate case of raw market prices.

### 3.3.2 Implementation

Within the Models component, there is no C++-level construct shared by every model.

`InterestRateCurve` has piecewise constant continuously compounded rates.
It comes with a friend `IRCurveCalibration` class which builds a curve repricing a
strip of market swap rates.

`HazardRateCurve` is extremely simplified, with a constant hazard rate.
It represents the default risk of a certain issuer in a certain currency.
Is has a `survivalProbability()` member function.
The `hazardRate()` non-member function computes discrete hazard rates.

`S3Model` is an implementation of the "building blocks for credit derivative pricing" of Philipp J. Schönbucher's
*Credit Derivatives Pricing Models*, chapter 3.
Its constructor moves a tenor structure of forward rates and hazard rates, as well as a recovery rate, into its member data.
Its member functions (`T`, `delta`, `B`, `Bbar`, `e`, `pi`, `F`) provide the building blocks in the notations of
the book.

## 3.4 Container

### 3.4.1 Objects and Identifiers

`Id` is the concept (in the C++20 sense) of object identifier.

The `Id` types are defined in the `Ids` folder. They are:

| `Id`                  | `ObjectType`        | Example
|-------------          | -------------       | -----------
| `PricerId`            | `Pricer`            | `Pricer{S3,[Instrument{0},Instrument{2}]}`
| `InstrumentId`        | `Instrument`        | `Instrument{0}`
| `HazardRateCurveId`   | `HazardRateCurve`   | `HazardRateCurve{USD,JPM}`
| `InterestRateCurveId` | `InterestRateCurve` | `InterestRateCurve{EUR}`
| `S3ModelId`           | `S3Model`           | `HazardRateCurve{USD,JPM}`

An `Id` instance must allow us to uniquely identify the role of an object in business-wise meaningful terms.
For example, the value `HazardRateCurveId{Currency::USD,Issuers::JPM}` identifies the hazard rate curve representing 
the default risk of issuer JP Morgan in USD.

`WithAllIdTypes` is a rather abstract construct (a template whose parameter is a template whose parameter is a type list)
which defines the list of `Id` types (as in the above table) and is used to define types involving all of the `Id` types.

For example, it is used to define `VariantId`, a `std::variant` whose alternatives are all of the `Id` types.

### 3.4.2 Container

`Container` contains objects of the `ObjectType` types, keyed by values of the `Id` types.
It uses `std::unique_ptr` so it assumes sole ownership of the objects it contains.

It provides member function templates `get()` and `set()`, parameterized by an `Id` type.

The `get()` member function template returns a raw pointer. 
This is standard for non-owning references which may be null. (Cf C++ Core Guidelines F.60.)

### 3.4.3 Container overlays

A `Container` instance `C1` may be an overlay on another `Container` instance `C2`. 
In that arrangement,
* all the elements of `C2` appear as elements of `C1`;
* `C1` may override the values of some of these elements;
* `C1` may also have extra elements.

This feature is a foundation for the efficient implementation of container state
scenarios and, from there, finite-difference greeks.

For example, we may have a large container with many different kinds of objects.
We want a scenario where the EUR IR curve is shifted up by 10bp, and all the objects
which depend on it are updated correspondingly.
For this, we create an overlay container referring to the original container, and we
override only `IRCurve{EUR}` and all its dependents.

This arrangement is in the spirit of the Decorator pattern.
It offers great flexibility and robustness (all objects are constant). 
Compared to in-place application of object state changes, it sacrifices some memory, but
opens the possibility of safe and efficient multi-threading.

### 3.4.4 Flexibility

`Container` is non-intrusive with respect to the types of the objects it may contain (e.g. it does not require them to inherit from a specific abstract base class).
For example, the `S3Model` type is a standalone implementation of Schönbucher's specification, unpolluted by any framework requirement.
So, we can easily integrate any object type in `Container`: instruments, models, static data representations,
configurations, etc.

Besides, as we have seen, we can easily generate scenarios on all of those data.
For example, we can generate scenarios on instrument parameters just as easily as on market data.

## 3.5 Elaboration

### 3.5.1 Presentation of the problem

Container elaboration is the process of assembling in a Container, from various data sources, all the
data required for a specific computation task.

For example, we may want to price a collection of instruments, with a given pricer selection strategy.

We will first have to examine the instruments, to assign them to the S3 or IR pricer according to the
pricer selection strategy.

Then we will need those pricers to indicate the model data required for the evaluation of those instruments:
the IR curve of the instrument currency for the IR pricers, the S3 model of the instrument issuer and currency
for the S3 pricers.

Then we will need to load these model data.
The IR curves may be directly available from the data sources, but the S3 models may have to be created
from an IR curve and a hazard rate curve. 
So, we will have to get the identifiers of the IR curves and hazard rate curves required for the creation
of the S3 models, load them, then create the S3 models.

At some point, we will have set up all these objects in the container.
Then, the elaboration process will be complete and we will be able to move on to evaluating
the instrument PVs and greeks.

The problem of container elaboration is easy to overlook in the early stages of developing a multi-asset
quant library.
Simple ad-hoc strategies are sufficient for many cases.
However, as the library grows and its uses become more varied and more complex, elaboration, if not treated
properly, may become a critical pain point.
By the time this becomes evident, retro-fitting a general solution to the existing code base is hard.
So, in my opinion, it is well worth properly analysing the general problem, and designing a robust solution,
early on.

### 3.5.2 Specifications

The following properties are desirable.
* Customisable data sources, e.g. retrieving the objects from a self-contained file or from a data base connection.
* Customisable object creation, e.g. retrieving a calibrated IR curve from a data source or calibrating the IR curve
from IR instrument prices.
* Related to the previous point, customisable dependency relationships, e.g. an IR curve depends on nothing if provided
in calibrated form, but depends on IR instrument prices if it is calibrated in the elaboration process.
* Multi-step determination of the data requests of a given object, e.g. a pricer having initially only the instrument
identifiers will have to first request the instrument objects and, after they are loaded and can be examined, request
the model objets required to price the instruments.
* Configurable grouping of data requests, e.g. if data should be loaded in bulk, rather than piecemeal, in the interest of efficient communication with the data sources.
* Customisable computation task: different tasks require different Container contents, e.g. a pricing task may require calibrated models while a calibration task may require instrument prices.

### 3.5.3 Implementation

The implementation involves a good deal of abstraction (e.g. `BuilderFactory` is more or less a template for factories of factories).
It is the price of generality.

The entry point is `elaborateContainer()`.
It consumes initial identifiers and a `BuilderGeneralFactory`, and returns
a Container containing the objects identified by the initial identifiers, and all the objects they request, and the
objects requested by those objects, and so on recursively.
It also returns a directed acyclical graph representing the *"object x requested object y during elaboration"* relationship.
This graph is useful for the propagation of object state changes, e.g. in greek implementations.

By calling `BuilderGeneralFactory`, one can create a `Builder` for a specific identifier.

A `Builder` is a state machine with a linear sequence of states. Each call to `getRequestBatch()` produces a batch
of identifiers requested for building the object managed by the builder, and advances the builder to the next state.
The final state is reached when `getRequestBatch()` produces an empty batch.
Then one must call `getObject()` to retrieve the built object.
Then
the `Builder` can be discarded.

The elaboration process is configured via the `BuilderGeneralFactory`. E.g. the switch between loading IR curves
from a data source in calibrated form and calibrating curves from instrument prices loaded from a data source, consists
in calling `BuilderGeneralFactory::setFactory<InterestRateCurveId>()` with different instances
of `BuilderFactory<InterestRateCurveId>`.

### 3.5.4 Example

The following graph shows the steps of an example elaboration.

Each node is labelled with 3 /-separated components:
* the *inclusion ordinal*, indicating the order in which objects are requested;
* the *population ordinal*, indicating the order in which the built objects are added to the container;
* the object identifier.

Each edge from object *x* to object *y* and labelled with number *i* means: *"object x requested object y in its i-th request batch"*.

![Elaboration](https://github.com/sdauby/credit-pricing/blob/main/svg/elaboration.svg "Elaboration")

## 3.6 Pricers

### 3.6.1 Pricer

`Pricer` is an interface for objects which manage the pricing of a collection of instruments.
The `pvs()` pure virtual function takes a `Container` reference and returns a map from `InstrumentId` to (pv,currency) pairs.

### 3.6.2 S3Pricer

`S3Pricer` is derived from `Pricer`. 
It is able to price fixed and floating coupon bonds and credit default swaps.
It uses the S3 model, i.e. Schönbucher's "building blocks", to derive instrument prices.

`S3Pricer` composes `S3UnitPricer`s, each of which is responsible for a single instrument.
They are created by the `makeS3UnitPricer()` factory function, where we see the `kind()` and `static_cast` pattern
mentioned in 3.2.1.
I prefer this pattern to having a `makeS3UnitPricer()` virtual function on the `Instrument` interface because in the
high-level architecture (see 2.2) the Instruments belong to a lower layer than the Pricers.
For example, we may want to build a PaymentProcessing component above Instruments, and that component would
have nothing to do with Pricers.
So, it would be wrong to encumber `Instrument` with pricer-related member functions.
The approach I have chosen is reminiscent of the Visitor design pattern.

The `aggregateTenorStructures()` function analyses all the `S3ModelId`s required by the `S3UnitPricer`s.
If two requirements differ only by their required tenor structure, those tenor structures are merged and
a single model having the merged tenor structure is created.
For example, this ensures that a 3Y CDS on JPM in USD and a 5Y CDS on JPM in USD use one S3 model rather
than two.
Although this example is a little contrived because of the extreme simplicity of the S3 model, it illustrates
that grouping instruments priced with the same method enables performance optimisations, something
that would not be possible if the `Pricer` interface were for single instruments rather than collections of
instruments.

### 3.6.3 IRPricer
`IRPricer`is derived from `Pricer`. It is able to price fixed and floating bonds and IR swaps.
It uses the `InterestRateCurve` model.

`IRPricer` is architecturally similar to `S3Pricer`.

## 3.7 Metrics

There are just two metrics at this point: `PV` and `IRDelta`.

The implementation of `IRDelta` is a straightforward combination of the features of `Pricer`, `Container` (overlays) and
`InterestRateCurve`.
It involves `propagateUpdate()` to rebuild the objects contained in the transitive closure of the shifted curves for the
*"x is requested by y during elaboration"* relation.

By configuring the `updateFunction` argument of `propagateUpdate()` we could implement more sophisticated scenarios 
and greeks than plain bump and rebuild, e.g.
* IR delta with respect to other instruments than those used in the original curve calibration;
* SABR-alpha-constant, normal-vol-constant, log-normal-vol-constant IR delta.

## 3.8 PricingEngine

`PricingEngine` is a namespace with a single function which produces instrument PVs and risks.

The `PricingConfiguration` class encapsulates the strategy for the selection of the pricer kind (IR or S3) to be applied to a given instrument.
In the example program, two strategies are applied successively:
* Use S3 for instruments which allow it, use IR otherwise (i.e. for IR swaps);
* Use IR for instruments which allow it, use S3 otherwise (i.e. for CDSs);

## 4. General comments

### 4.1 Object ownership model

All the objects allocated on the free-store are placed in `std::unique_ptr`s and moved into 
containers retaining sole ownership, e.g. Container.

Read-only access to these objects is provided by:
* the containers, in the form of constant reference return values
(sometimes in the form of constant pointer return values
to express that a null value is possible),
* constant reference arguments of functions,
* constant references stored in classes, initialized in the class constructors from constant reference arguments.

In all those cases, the references must not outlive the owning containers.
This is very easily achieved, without recourse to `shared_ptr`s.
This approach is consistent with C++ Core Guidelines R.21, F.7, R.37.

### 4.2 `Id` types

`Id` types are functionally fine as they are but not optimal for efficiency.
In particular, `Id` types involving free-store allocations (e.g. `S3ModelId`)
are not ideal value-semantics types.
This problem could be solved by representing the identifiers with an integer token
mapping to the actual content of the identifier.
