#pragma once

#include <memory>

class Container;

class Mutation {
public:
    virtual ~Mutation() = default;
    Mutation() = default;
    Mutation(const Mutation&) = delete;
    Mutation& operator=(const Mutation&) = delete;

    // The output modelContainer has a non-owning reference to the input modelContainer: the
    // caller must ensure that the lifetime of the input modelContainer is greater than the
    // lifetime of the output modelContainer.
    virtual std::unique_ptr<Container> apply(const Container& container) const = 0;
};
