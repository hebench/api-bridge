
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <cstring>
#include <stdexcept>

#include "hebench/api_bridge/cpp/benchmark.hpp"

namespace hebench {
namespace cpp {

//----------------------------
// class BenchmarkDescription
//----------------------------

std::string BenchmarkDescription::getBenchmarkDescription(const hebench::APIBridge::WorkloadParams *p_w_params) const
{
    (void)p_w_params;
    return std::string();
}

std::size_t BenchmarkDescription::getWorkloadParameterCount() const
{
    return m_default_params.empty() ? 0 : m_default_params.front().size();
}

void BenchmarkDescription::getBenchmarkDescriptor(hebench::APIBridge::BenchmarkDescriptor &bench_desc) const
{
    bench_desc = getBenchmarkDescriptor();
}

void BenchmarkDescription::addDefaultParameters(const WorkloadParams::Common &default_params_set)
{
    addDefaultParameters(default_params_set.getParams());
}

void BenchmarkDescription::addDefaultParameters(const std::vector<hebench::APIBridge::WorkloadParam> &default_params_set)
{
    if (!m_default_params.empty())
    {
        if (m_default_params.front().size() != default_params_set.size())
            throw std::invalid_argument("Size of new default set of arguments differs from other existing default sets.");
    } // end if
    m_default_params.push_back(default_params_set);
}

//---------------------
// class BaseBenchmark
//---------------------

BaseBenchmark::BaseBenchmark(BaseEngine &engine,
                             const hebench::APIBridge::BenchmarkDescriptor &bench_desc) :
    m_engine(engine),
    m_bench_description(bench_desc)
{
}

BaseBenchmark::BaseBenchmark(BaseEngine &engine,
                             const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                             const hebench::APIBridge::WorkloadParams &bench_params) :
    BaseBenchmark(engine, bench_desc)
{
    if (bench_params.count > 0)
        m_bench_params.assign(bench_params.params, bench_params.params + bench_params.count);
}

std::uint64_t BaseBenchmark::findDataPackIndex(const hebench::APIBridge::PackedData &parameters,
                                               std::uint64_t param_position)
{
    std::uint64_t retval = parameters.pack_count;

    if (parameters.p_data_packs)
    {
        for (std::uint64_t i = 0; retval >= parameters.pack_count && i < parameters.pack_count; ++i)
            if (parameters.p_data_packs[i].param_position == param_position)
                retval = i;
    } // end if

    return retval;
}

const hebench::APIBridge::DataPack &BaseBenchmark::findDataPack(const hebench::APIBridge::PackedData &parameters,
                                                                std::uint64_t param_position)
{
    std::uint64_t tmp_u64 = findDataPackIndex(parameters, param_position);
    if (tmp_u64 >= parameters.pack_count)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("DataPack for component " + std::to_string(param_position) + " not found."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    return parameters.p_data_packs[tmp_u64];
}

hebench::APIBridge::DataPack &BaseBenchmark::findDataPack(hebench::APIBridge::PackedData &parameters,
                                                          std::uint64_t param_position)
{
    std::uint64_t tmp_u64 = findDataPackIndex(parameters, param_position);
    if (tmp_u64 >= parameters.pack_count)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("DataPack for component " + std::to_string(param_position) + " not found."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    return parameters.p_data_packs[tmp_u64];
}

} // namespace cpp
} // namespace hebench
