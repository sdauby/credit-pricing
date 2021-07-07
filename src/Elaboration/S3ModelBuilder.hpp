#pragma once

#include "Elaboration/Builder.hpp"

class S3ModelBuilder : public Builder<S3ModelId> {
public:
    S3ModelBuilder(const S3ModelId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<S3Model> getObject(const Container& container) override;

    S3ModelId id_;
    enum class State {
        Initial,
        CurvesRequested,
        Final,
    };
    State state_ = State::Initial;
};
