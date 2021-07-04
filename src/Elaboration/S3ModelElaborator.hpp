#pragma once

#include "Elaborator.hpp"

class S3ModelElaborator : public Elaborator<S3ModelId> {
public:
    S3ModelElaborator(const S3ModelId& id);

private:
    std::vector<VariantId> getRequestBatch(const Container&) override;
    std::unique_ptr<S3Model> make(const Container& container) override;

    S3ModelId id_;
    enum class State {
        Initial,
        CurvesRequested,
        Final,
    };
    State state_ = State::Initial;
};
