
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <array>

#include "hebench/api_bridge/cpp/hebench.hpp"

class ExampleEngine;

class ExampleBenchmarkDescription : public hebench::cpp::BenchmarkDescription
{
public:
    HEBERROR_DECLARE_CLASS_NAME(ExampleBenchmarkDescription)

public:
    // This workload (MatrixMultiply) requires only 3 configurable parameters
    static constexpr std::uint64_t NumWorkloadParams = 3;
    // This workload has 2 operands.
    static constexpr std::uint64_t NumOperands = 2;
    // This workload result has only 1 component (the resulting matrix).
    static constexpr std::uint64_t NumOpResultComponents = 1;
    ExampleBenchmarkDescription();
    ~ExampleBenchmarkDescription() override;

    hebench::cpp::BaseBenchmark *createBenchmark(hebench::cpp::BaseEngine &engine,
                                                 const hebench::APIBridge::WorkloadParams *p_params) override;
    void destroyBenchmark(hebench::cpp::BaseBenchmark *p_bench) override;

    std::string getBenchmarkDescription(const hebench::APIBridge::WorkloadParams *p_w_params) const override;
};

class ExampleBenchmark : public hebench::cpp::BaseBenchmark
{
public:
    HEBERROR_DECLARE_CLASS_NAME(ExampleBenchmark)

public:
    static constexpr std::int64_t tag = 0x1;

    ExampleBenchmark(ExampleEngine &engine,
                     const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                     const hebench::APIBridge::WorkloadParams &bench_params);
    ~ExampleBenchmark() override;

    void initialize(const hebench::APIBridge::BenchmarkDescriptor &bench_desc_concrete) override;
    hebench::APIBridge::Handle encode(const hebench::APIBridge::DataPackCollection *p_parameters) override;
    void decode(hebench::APIBridge::Handle encoded_data, hebench::APIBridge::DataPackCollection *p_native) override;
    hebench::APIBridge::Handle encrypt(hebench::APIBridge::Handle encoded_data) override;
    hebench::APIBridge::Handle decrypt(hebench::APIBridge::Handle encrypted_data) override;

    hebench::APIBridge::Handle load(const hebench::APIBridge::Handle *p_local_data, std::uint64_t count) override;
    void store(hebench::APIBridge::Handle remote_data,
               hebench::APIBridge::Handle *p_local_data, std::uint64_t count) override;

    hebench::APIBridge::Handle operate(hebench::APIBridge::Handle h_remote_packed,
                                       const hebench::APIBridge::ParameterIndexer *p_param_indexers,
                                       std::uint64_t indexers_count) override;

    std::int64_t classTag() const override { return BaseBenchmark::classTag() | ExampleBenchmark::tag; }

private:
    static constexpr std::int64_t tagEncodeOutput  = 0x10;
    static constexpr std::int64_t tagDecodeOutput  = 0x20;
    static constexpr std::int64_t tagEncryptOutput = 0x40;
    static constexpr std::int64_t tagDecryptOutput = 0x80;
    static constexpr std::int64_t tagLoadOutput    = 0x100;
    static constexpr std::int64_t tagStoreOutput   = 0x200;
    static constexpr std::int64_t tagOperateOutput = 0x400;

    struct Matrix
    {
        std::array<std::array<double, 100>, 100> rows;
    };
};
