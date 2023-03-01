
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <cassert>
#include <new>
#include <sstream>

#include "hebench/api_bridge/cpp/benchmark.hpp"
#include "hebench/api_bridge/cpp/engine.hpp"
#include "hebench/api_bridge/cpp/error_handling.hpp"

namespace hebench {
namespace cpp {

//------------------
// class BaseEngine
//------------------

const std::string BaseEngine::UnknownErrorMsg          = "Unknown Error";
hebench::APIBridge::ErrorCode BaseEngine::m_last_error = HEBENCH_ECODE_SUCCESS;
std::string BaseEngine::m_s_last_error_description;
std::unordered_map<hebench::APIBridge::ErrorCode, std::string> BaseEngine::m_map_error_desc = {
    { HEBENCH_ECODE_SUCCESS, "Success" },
    { HEBENCH_ECODE_INVALID_ARGS, "Invalid argument." },
    { HEBENCH_ECODE_CRITICAL_ERROR, "Critical error." }
};

BaseEngine::BaseEngine()
{
    //
}

const std::string &BaseEngine::getErrorDesc(hebench::APIBridge::ErrorCode err_code)
{
    auto it = m_map_error_desc.find(err_code);
    if (it == m_map_error_desc.end())
        return UnknownErrorMsg;
    else
        return it->second;
}

void BaseEngine::setLastError(hebench::APIBridge::ErrorCode value)
{
    setLastError(value, getErrorDesc(value));
}

void BaseEngine::setLastError(hebench::APIBridge::ErrorCode value,
                              const std::string &err_desc)
{
    m_last_error               = value;
    m_s_last_error_description = err_desc;
}

std::string BaseEngine::getBenchmarkDescriptionEx(hebench::APIBridge::Handle h_bench_desc,
                                                  const hebench::APIBridge::WorkloadParams *p_w_params) const
{
    std::shared_ptr<BenchmarkDescription> p_bd = matchBenchmark(h_bench_desc);
    if (!p_bd)
        throw HEBenchError(HEBERROR_MSG_CLASS("Invalid benchmark descriptor not matched."),
                           HEBENCH_ECODE_CRITICAL_ERROR);

    return p_bd->getBenchmarkDescription(p_w_params);
}

void BaseEngine::addBenchmarkDescription(std::shared_ptr<BenchmarkDescription> p_desc)
{
    m_descriptors.push_back(p_desc);
}

void BaseEngine::addErrorCode(hebench::APIBridge::ErrorCode code, const std::string &description)
{
    if (m_map_error_desc.count(code) > 0)
        throw HEBenchError(HEBERROR_MSG_CLASS("Error code already specified."),
                           HEBENCH_ECODE_CRITICAL_ERROR);
    m_map_error_desc[code] = description;
}

void BaseEngine::addSchemeName(hebench::APIBridge::Scheme scheme, const std::string &name)
{
    if (m_map_scheme_name.count(scheme) > 0)
        throw HEBenchError(HEBERROR_MSG_CLASS("Scheme already specified."),
                           HEBENCH_ECODE_CRITICAL_ERROR);
    m_map_scheme_name[scheme] = name;
}

void BaseEngine::addSecurityName(hebench::APIBridge::Security security, const std::string &name)
{
    if (m_map_security_name.count(security) > 0)
        throw HEBenchError(HEBERROR_MSG_CLASS("Security type already specified."),
                           HEBENCH_ECODE_CRITICAL_ERROR);
    m_map_security_name[security] = name;
}

std::shared_ptr<BenchmarkDescription> BaseEngine::matchBenchmark(hebench::APIBridge::Handle h_desc) const
{
    std::shared_ptr<BenchmarkDescription> p_retval;
    std::size_t index = (std::size_t)(h_desc.p);
    if (index < m_descriptors.size() && (h_desc.tag & BenchmarkDescription::tag) != 0)
        p_retval = m_descriptors.at(index);

    return p_retval;
}

void BaseEngine::subscribeBenchmarks(hebench::APIBridge::Handle *p_h_bench_descs, std::uint64_t count) const
{
    if (!p_h_bench_descs)
        throw HEBenchError(HEBERROR_MSG_CLASS("Invalid null parameter: `p_h_bench_descs`."),
                           HEBENCH_ECODE_CRITICAL_ERROR);
    if (count != static_cast<std::uint64_t>(m_descriptors.size()))
    {
        std::stringstream ss;
        ss << "Invalid size parameter: `count`. Expected " << m_descriptors.size()
           << ", but " << count << " received.";
        throw HEBenchError(HEBERROR_MSG_CLASS(ss.str()),
                           HEBENCH_ECODE_INVALID_ARGS);
    } // end if

    std::uint64_t min_size = std::min(count, static_cast<std::uint64_t>(m_descriptors.size()));
    assert(min_size == static_cast<std::uint64_t>(m_descriptors.size()));
    for (std::size_t i = 0; i < min_size; ++i)
    {
        p_h_bench_descs[i].p    = (void *)(i);
        p_h_bench_descs[i].size = sizeof(BenchmarkDescription);
        p_h_bench_descs[i].tag  = m_descriptors[i]->classTag();
    } // end for
}

std::uint64_t BaseEngine::getWorkloadParamsCount(hebench::APIBridge::Handle h_bench_desc) const
{
    std::shared_ptr<BenchmarkDescription> p_bd = matchBenchmark(h_bench_desc);
    if (!p_bd)
        throw HEBenchError(HEBERROR_MSG_CLASS("Invalid benchmark descriptor not matched."),
                           HEBENCH_ECODE_CRITICAL_ERROR);
    return p_bd->getWorkloadParameterCount();
}

std::uint64_t BaseEngine::getDefaultWorkloadParamsCount(hebench::APIBridge::Handle h_bench_desc) const
{
    std::shared_ptr<BenchmarkDescription> p_bd = matchBenchmark(h_bench_desc);
    if (!p_bd)
        throw HEBenchError(HEBERROR_MSG_CLASS("Invalid benchmark descriptor not matched."),
                           HEBENCH_ECODE_CRITICAL_ERROR);
    return p_bd->getWorkloadDefaultParameters().size();
}

void BaseEngine::describeBenchmark(hebench::APIBridge::Handle h_bench_desc,
                                   hebench::APIBridge::BenchmarkDescriptor *p_bench_desc,
                                   hebench::APIBridge::WorkloadParams *p_default_params,
                                   std::uint64_t default_count) const
{
    if (!p_bench_desc)
        throw HEBenchError(HEBERROR_MSG_CLASS("Invalid null parameter: p_bench_desc"),
                           HEBENCH_ECODE_CRITICAL_ERROR);

    std::shared_ptr<BenchmarkDescription> p_bd = matchBenchmark(h_bench_desc);
    if (!p_bd)
        throw HEBenchError(HEBERROR_MSG_CLASS("Invalid benchmark descriptor not matched."),
                           HEBENCH_ECODE_CRITICAL_ERROR);

    p_bd->getBenchmarkDescriptor(*p_bench_desc);

    if (p_default_params)
    {
        // return the default parameters
        const auto &default_params      = p_bd->getWorkloadDefaultParameters();
        std::uint64_t min_default_count = std::min<std::uint64_t>(default_count, default_params.size());
        for (std::size_t i = 0; i < min_default_count; ++i)
        {
            // all WorkloadParams elements in p_default_params must be pre-allocated by caller.
            if (p_default_params[i].count < default_params[i].size())
                throw HEBenchError(HEBERROR_MSG_CLASS("Insufficient space allocated for default set of parameters: " + std::to_string(i)),
                                   HEBENCH_ECODE_CRITICAL_ERROR);
            std::copy(default_params[i].begin(), default_params[i].end(), p_default_params[i].params);
        } // end for
    } // end if
}

hebench::APIBridge::Handle BaseEngine::createBenchmark(hebench::APIBridge::Handle h_bench_desc,
                                                       const hebench::APIBridge::WorkloadParams *p_params)
{
    std::shared_ptr<BenchmarkDescription> p_bd = matchBenchmark(h_bench_desc);
    if (!p_bd)
        throw HEBenchError(HEBERROR_MSG_CLASS("Invalid benchmark descriptor not matched."),
                           HEBENCH_ECODE_CRITICAL_ERROR);

    BaseBenchmark *p_bench    = p_bd->createBenchmark(*this, p_params);
    BenchmarkHandle *p_bh     = this->template createObj<BenchmarkHandle>();
    p_bh->p_benchmark         = p_bench;
    p_bh->p_bench_description = p_bd.get();
    hebench::APIBridge::Handle retval;
    retval.p    = p_bh;
    retval.size = sizeof(BenchmarkHandle);
    retval.tag  = p_bench->classTag() | BaseBenchmark::tag | BenchmarkDescription::tag;
    return retval;
}

void BaseEngine::destroyBenchmark(hebench::APIBridge::Handle h_bench)
{
    if (h_bench.p)
    {
        if ((h_bench.tag & (BaseBenchmark::tag | BenchmarkDescription::tag)) == 0)
            throw HEBenchError(HEBERROR_MSG_CLASS("Invalid handle not matching benchmark type."),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BenchmarkHandle *p_bh = reinterpret_cast<BenchmarkHandle *>(h_bench.p);
        if (!p_bh->p_benchmark || !p_bh->p_bench_description)
            throw HEBenchError(HEBERROR_MSG_CLASS("Invalid empty handle."),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        p_bh->p_bench_description->destroyBenchmark(p_bh->p_benchmark);
        this->template destroyObj<BenchmarkHandle>(p_bh);
    } // end if
}

void BaseEngine::checkHandleTags(hebench::APIBridge::Handle h, std::int64_t check_tags) const
{
    if ((check_tags & ITaggedObject::MaskReservedBits) != 0)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid `check_tags` detected. Most significant 8 bits of tags are reserved."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    if ((h.tag & hebench::cpp::EngineObject::tag) != hebench::cpp::EngineObject::tag)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid tag detected. Expected EngineObject::tag."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    if ((h.tag & check_tags) != check_tags)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid tag detected. Expected " + std::to_string(check_tags) + "."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
}

hebench::APIBridge::Handle BaseEngine::duplicateHandle(hebench::APIBridge::Handle h, std::int64_t new_tags, std::int64_t check_tags) const
{
    checkHandleTags(h, check_tags);
    if (new_tags != h.tag && (new_tags & ITaggedObject::MaskReservedBits) != 0)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid `new_tags` detected. Most significant 8 bits of tags are reserved."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);

    return duplicateHandleInternal(h, new_tags);
}

hebench::APIBridge::Handle BaseEngine::duplicateHandle(hebench::APIBridge::Handle h, std::int64_t check_tags) const
{
    checkHandleTags(h, check_tags);
    return duplicateHandleInternal(h, h.tag);
}

hebench::APIBridge::Handle BaseEngine::duplicateHandleInternal(hebench::APIBridge::Handle h, std::int64_t new_tag) const
{
    if (!h.p)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid null handle."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);

    // retrieve our internal format object from the handle
    hebench::cpp::EngineObject *p_obj = reinterpret_cast<hebench::cpp::EngineObject *>(h.p);
    assert(p_obj);
    if (this != &p_obj->engine())
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid handle. Handle was not created by invoked engine."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    // copy internal object
    hebench::cpp::EngineObject *p_retval = new EngineObject(*p_obj);
    if (!p_retval)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Allocation failed."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    hebench::APIBridge::Handle retval;
    retval.p    = p_retval;
    retval.size = h.size;
    retval.tag  = p_retval->classTag() | new_tag;
    return retval;
}

} // namespace cpp
} // namespace hebench
