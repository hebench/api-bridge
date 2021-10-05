
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_Benchmark_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_Benchmark_H_7e5fa8c2415240ea93eff148ed73539b

#include <cstdint>
#include <memory>
#include <vector>

#include "engine_object.hpp"
#include "error_handling.hpp"
#include "hebench/api_bridge/types.h"
#include "workload_params.hpp"

namespace hebench {
namespace cpp {

class BaseEngine;
class BaseBenchmark;

/**
 * @brief Base class for objects representing a specific benchmark based on an
 * HEBench benchmark descriptor.
 * @details
 * An HEBench benchmark descriptor describes a unique benchmark and specifies
 * default workload parameters supported.
 *
 * This class wraps the functionality to describe, instantiate and destroy the
 * appropriate benchmark class. A BaseEngine (or derived class) uses this
 * behavior to create the appropriate benchmark object that can execute the
 * workload requested by Test Harness.
 */
class BenchmarkDescription : public ITaggedObject
{
public:
    /**
     * @brief Used to identify this class when returned as a handle to Test harness.
     * @sa ITaggedObject
     */
    static constexpr std::int64_t tag = 0x1000000000000000; // bit 60
public:
    BenchmarkDescription()          = default;
    virtual ~BenchmarkDescription() = default;

    /**
     * @brief Retrieves the HEBench benchmark descriptor represented by
     * this BenchmarkDescription object.
     * @param[out] bench_desc Reference where to store the benchmark descriptor.
     */
    void getBenchmarkDescriptor(hebench::APIBridge::BenchmarkDescriptor &bench_desc) const;
    /**
     * @brief Retrieves the HEBench benchmark descriptor represented by
     * this BenchmarkDescription object.
     * @return A reference to the benchmark descriptor.
     */
    const hebench::APIBridge::BenchmarkDescriptor &getBenchmarkDescriptor() const { return m_descriptor; }
    /**
     * @brief Retrieves the sets of default arguments supported by this benchmark's workload.
     * @return A collection of sets of default arguments supported by this benchmark's workload.
     * The collection is empty if this workload does not have parameters.
     */
    const std::vector<std::vector<hebench::APIBridge::WorkloadParam>> &getWorkloadDefaultParameters() const { return m_default_params; }
    /**
     * @brief Retrieves human-readable description specific to the represented benchmark.
     * @return A string containing the human-readable description for the benchmark as
     * specified by `hebench::APIBridge::getBenchmarkDescriptionEx()`.
     * @details Default implementation returns an empty string. Derived classes should
     * override this method and return appropriate description. The description will
     * be inserted in a CSV formatted output, so, derived classes can take advantage of
     * this knowledge to format the description as appropriate.
     * @sa hebench::APIBridge::getBenchmarkDescriptionEx()
     */
    virtual std::string getBenchmarkDescription(const hebench::APIBridge::WorkloadParams *p_w_params) const;

    /**
     * @brief Retrieves the number of flexible parameters for this benchmark's workload.
     * @return The number of flexible parameters for this benchmark's workload, or 0
     * if no parameters.
     * @details This is computed based on default parameters added to the description
     * using addDefaultParameters().
     */
    std::size_t getWorkloadParameterCount() const;
    /**
     * @brief Instantiates the represented benchmark.
     * @param engine Engine creating the benchmark.
     * @param[in] p_params Arguments to the workload parameters to use for this benchmark.
     * It is ignored if workload does not support parameters.
     * @return A pointer to the base benchmark class of the instantiated benchmark
     * to be wrapped by a handle that will cross the API Bridge boundary.
     */
    virtual BaseBenchmark *createBenchmark(BaseEngine &engine,
                                           const hebench::APIBridge::WorkloadParams *p_params) = 0;
    /**
     * @brief Destroys and frees resources held by a BaseBenchmark (or derived)
     * object previously created by createBenchmark().
     * @param[in] p_bench Pointer to benchmark object to destroy.
     */
    virtual void destroyBenchmark(BaseBenchmark *p_bench) = 0;

    std::int64_t classTag() const override { return BenchmarkDescription::tag; }

protected:
    /**
     * @brief HEBench descriptor for this benchmark.
     * @details This member is to be initialized during derived class construction
     * to reflect the appropriate benchmark description.
     */
    hebench::APIBridge::BenchmarkDescriptor m_descriptor;

    /**
     * @brief Adds a new set of default arguments for the parameters for this
     * benchmark's workload.
     * @param[in] default_params_set The new set of default values for the parameters
     * of this workload.
     * @throws std::invalid_argument if size of \p default_params_set is invalid. See details.
     * @details If this is not the first set added, \p default_params_set must represent a
     * set with the same amount of parameters as any other existing default parameter sets.
     */
    void addDefaultParameters(const WorkloadParams::Generic &default_params_set);
    /**
     * @brief Adds a new set of default arguments for the parameters for this
     * benchmark's workload.
     * @param[in] default_params_set The new set of default values for the parameters
     * of this workload.
     * @throws std::invalid_argument if size of \p default_params_set is invalid. See details.
     * @details If this is not the first set added, the size of \p default_params_set must be
     * the same as any other existing default parameter sets.
     */
    void addDefaultParameters(const std::vector<hebench::APIBridge::WorkloadParam> &default_params_set);

private:
    std::vector<std::vector<hebench::APIBridge::WorkloadParam>> m_default_params;
};

/**
 * @brief Top level opaque benchmark class.
 * @details This class hides the internal complexities of the back-end operations from
 * the API Bridge.
 */
class BaseBenchmark : public ITaggedObject
{
private:
    HEBERROR_DECLARE_CLASS_NAME(BaseBenchmark)

public:
    /**
     * @brief Used to identify this class when returned as a handle to Test harness.
     * @sa ITaggedObject
     */
    static constexpr std::int64_t tag = 0x4000000000000000; // bit 62

    /**
     * @brief Constructs a new BaseBenchmark object for benchmarking a workload
     * with flexible parameters.
     * @param engine Calling engine.
     * @param[in] bench_desc Calling description.
     * @param[in] bench_params Parameters for this benchmark's workload.
     */
    BaseBenchmark(BaseEngine &engine,
                  const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                  const hebench::APIBridge::WorkloadParams &bench_params);
    /**
     * @brief Constructs a new BaseBenchmark object for benchmarking a workload
     * that does not have any flexible parameters.
     * @param engine Calling engine.
     * @param[in] bench_desc Calling description.
     */
    BaseBenchmark(BaseEngine &engine,
                  const hebench::APIBridge::BenchmarkDescriptor &bench_desc);
    virtual ~BaseBenchmark() = default;

    virtual hebench::APIBridge::Handle encode(const hebench::APIBridge::PackedData *p_parameters)          = 0;
    virtual void decode(hebench::APIBridge::Handle encoded_data, hebench::APIBridge::PackedData *p_native) = 0;
    virtual hebench::APIBridge::Handle encrypt(hebench::APIBridge::Handle encoded_data)                    = 0;
    virtual hebench::APIBridge::Handle decrypt(hebench::APIBridge::Handle encrypted_data)                  = 0;

    virtual hebench::APIBridge::Handle load(const hebench::APIBridge::Handle *p_local_data, std::uint64_t count)              = 0;
    virtual void store(hebench::APIBridge::Handle remote_data, hebench::APIBridge::Handle *p_local_data, std::uint64_t count) = 0;

    virtual hebench::APIBridge::Handle operate(hebench::APIBridge::Handle h_remote_packed,
                                               const hebench::APIBridge::ParameterIndexer *p_param_indexers) = 0;

    BaseEngine &getEngine() { return m_engine; }
    const BaseEngine &getEngine() const { return m_engine; }

    std::int64_t classTag() const override { return BaseBenchmark::tag; }

    const hebench::APIBridge::BenchmarkDescriptor &getDescriptor() const { return m_bench_description; }
    const std::vector<hebench::APIBridge::WorkloadParam> &getWorkloadParameters() const { return m_bench_params; }

protected:
    /**
     * @brief Searches the specified PackedData for the DataPack in the corresponding position.
     * @param[in] parameters PackedData to search.
     * @param[in] param_position Component position of the DataPack to find inside `parameters`.
     * @return The index of the DataPack inside `parameters` corresponding to the
     * specified `param_position` or parameters.pack_count if no DataPack for the
     * specified `param_position` is found.
     */
    static std::uint64_t findDataPackIndex(const hebench::APIBridge::PackedData &parameters,
                                           std::uint64_t param_position);
    /**
     * @brief Searches the specified PackedData for the DataPack in the corresponding position.
     * @param[in] parameters PackedData to search.
     * @param[in] param_position Component position of the DataPack to find inside `parameters`.
     * @return A reference to the DataPack inside `parameters` corresponding to the
     * specified `param_position`.
     * @throws hebench::cpp::HEBenchError if no DataPack for the specified `param_position`
     * is found.
     */
    static const hebench::APIBridge::DataPack &findDataPack(const hebench::APIBridge::PackedData &parameters,
                                                            std::uint64_t param_position);
    /**
     * @brief Searches the specified PackedData for the DataPack in the corresponding position.
     * @param[in] parameters PackedData to search.
     * @param[in] param_position Component position of the DataPack to find inside `parameters`.
     * @return A reference to the DataPack inside `parameters` corresponding to the
     * specified `param_position`.
     * @throws hebench::cpp::HEBenchError if no DataPack for the specified `param_position`
     * is found.
     */
    static hebench::APIBridge::DataPack &findDataPack(hebench::APIBridge::PackedData &parameters,
                                                      std::uint64_t param_position);
    void setDescriptor(const hebench::APIBridge::BenchmarkDescriptor &value) { m_bench_description = value; }

private:
    BaseEngine &m_engine;
    hebench::APIBridge::BenchmarkDescriptor m_bench_description;
    std::vector<hebench::APIBridge::WorkloadParam> m_bench_params;
};

} // namespace cpp
} // namespace hebench

#endif // defined _HEBench_API_Bridge_Benchmark_H_7e5fa8c2415240ea93eff148ed73539b
