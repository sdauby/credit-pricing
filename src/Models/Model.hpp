#pragma once

class Model {
public:
    virtual ~Model() = default;
    Model() = default;
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
};
