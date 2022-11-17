
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <cstring>
#include <stdexcept>

#include "hebench/api_bridge/api.h"
#include "hebench/api_bridge/cpp/hebench.hpp"

using namespace hebench::cpp;

namespace hebench {
namespace APIBridge {

bool checkHandleEquals(const Handle &h, std::int64_t tag)
{
    return (h.p != nullptr) && (h.tag == tag);
}

bool checkHandleBits(const Handle &h, std::int64_t tag)
{
    // check if handle contains the bits of the tag
    return (h.p != nullptr) && ((h.tag & tag) == tag);
}

ErrorCode destroyHandle(Handle h)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (h.p)
        {
            if (checkHandleBits(h, BaseBenchmark::tag | BenchmarkDescription::tag))
            {
                BenchmarkHandle *p_bh = reinterpret_cast<BenchmarkHandle *>(h.p);
                BaseEngine &engine    = p_bh->p_benchmark->getEngine();
                engine.destroyBenchmark(h);
            } // end if
            else if (checkHandleBits(h, EngineObject::tag))
            {
                EngineObject *p_obj      = reinterpret_cast<EngineObject *>(h.p);
                const BaseEngine &engine = p_obj->engine();
                engine.destroyObj<EngineObject>(p_obj);
            } // end else if
            else
            {
                // check for exact matches
                switch (h.tag)
                {
                case BaseEngine::tag:
                {
                    BaseEngine *p_engine = reinterpret_cast<BaseEngine *>(h.p);
                    destroyEngine(p_engine);
                }
                break;
                case BenchmarkDescription::tag:
                    // A handle for BenchmarkDescription is just an index inside the
                    // vector of descriptions, so, it does not need to be released.
                    break;

                default:
                    throw HEBenchError(HEBERROR_MSG("Invalid tag in handle."),
                                       HEBENCH_ECODE_CRITICAL_ERROR);
                    break;
                } // end switch
            } // end else
        } // end if
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode initEngine(Handle *h_engine, const int8_t *p_buffer, uint64_t size)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!h_engine)
            throw HEBenchError(HEBERROR_MSG("Invalid null handle 'h_engine'."),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BaseEngine *p_engine = createEngine(p_buffer, size);
        h_engine->p          = p_engine;
        h_engine->size       = sizeof(BaseEngine);
        h_engine->tag        = p_engine->classTag();
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode subscribeBenchmarksCount(Handle h_engine, std::uint64_t *p_count)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!checkHandleBits(h_engine, BaseEngine::tag))
            throw HEBenchError(HEBERROR_MSG("Invalid handle: h_engine"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!p_count)
            throw HEBenchError(HEBERROR_MSG("Invalid null parameter: p_count"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BaseEngine *p_engine = reinterpret_cast<BaseEngine *>(h_engine.p);
        *p_count             = p_engine->subscribeBenchmarkCount();
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode subscribeBenchmarks(Handle h_engine, Handle *p_h_bench_descs)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!checkHandleBits(h_engine, BaseEngine::tag))
            throw HEBenchError(HEBERROR_MSG("Invalid handle: h_engine"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!p_h_bench_descs)
            throw HEBenchError(HEBERROR_MSG("Invalid null parameter: p_bench_descs"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BaseEngine *p_engine = reinterpret_cast<BaseEngine *>(h_engine.p);
        p_engine->subscribeBenchmarks(p_h_bench_descs);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode getWorkloadParamsDetails(Handle h_engine,
                                   Handle h_bench_desc,
                                   std::uint64_t *p_param_count,
                                   std::uint64_t *p_default_count)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!checkHandleBits(h_engine, BaseEngine::tag))
            throw HEBenchError(HEBERROR_MSG("Invalid handle: h_engine"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!p_param_count)
            throw HEBenchError(HEBERROR_MSG("Invalid null parameter: p_param_count"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!p_default_count)
            throw HEBenchError(HEBERROR_MSG("Invalid null parameter: p_default_count"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BaseEngine *p_engine = reinterpret_cast<BaseEngine *>(h_engine.p);
        *p_param_count       = p_engine->getWorkloadParamsCount(h_bench_desc);
        *p_default_count     = p_engine->getDefaultWorkloadParamsCount(h_bench_desc);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode describeBenchmark(Handle h_engine,
                            Handle h_bench_desc,
                            BenchmarkDescriptor *p_bench_desc,
                            WorkloadParams *p_default_params,
                            std::uint64_t default_count)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!checkHandleBits(h_engine, BaseEngine::tag))
            throw HEBenchError(HEBERROR_MSG("Invalid handle: h_engine"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!p_bench_desc)
            throw HEBenchError(HEBERROR_MSG("Invalid null parameter: p_bench_desc"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BaseEngine *p_engine = reinterpret_cast<BaseEngine *>(h_engine.p);
        p_engine->describeBenchmark(h_bench_desc, p_bench_desc, p_default_params, default_count);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode createBenchmark(Handle h_engine,
                          Handle h_bench_desc,
                          const WorkloadParams *p_params,
                          Handle *h_benchmark)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!checkHandleBits(h_engine, BaseEngine::tag))
            throw HEBenchError(HEBERROR_MSG("Invalid handle: h_engine"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_benchmark)
            throw HEBenchError(HEBERROR_MSG("Invalid null handle: h_benchmark"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BaseEngine *p_engine = reinterpret_cast<BaseEngine *>(h_engine.p);
        *h_benchmark         = p_engine->createBenchmark(h_bench_desc, p_params);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode initBenchmark(Handle h_benchmark,
                        const BenchmarkDescriptor *p_concrete_desc)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!h_benchmark.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_benchmark'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!p_concrete_desc)
            throw HEBenchError(HEBERROR_MSG("Invalid null benchmark descriptor 'p_concrete_desc'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BenchmarkHandle *p_bh = reinterpret_cast<BenchmarkHandle *>(h_benchmark.p);
        p_bh->p_benchmark->initialize(*p_concrete_desc);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode encode(Handle h_benchmark, const DataPackCollection *p_parameters, Handle *h_plaintext)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!h_benchmark.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_benchmark'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!p_parameters || (p_parameters->pack_count > 0 && !p_parameters->p_data_packs))
            throw HEBenchError(HEBERROR_MSG("Invalid null packed data 'p_parameters'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_plaintext)
            throw HEBenchError(HEBERROR_MSG("Invalid null handle 'h_plaintext'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BenchmarkHandle *p_bh = reinterpret_cast<BenchmarkHandle *>(h_benchmark.p);
        *h_plaintext          = p_bh->p_benchmark->encode(p_parameters);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode decode(Handle h_benchmark, Handle h_plaintext, DataPackCollection *p_native)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!h_benchmark.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_benchmark'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_plaintext.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_plaintext'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!p_native)
            throw HEBenchError(HEBERROR_MSG("Invalid null argument 'p_native'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BenchmarkHandle *p_bh = reinterpret_cast<BenchmarkHandle *>(h_benchmark.p);
        p_bh->p_benchmark->decode(h_plaintext, p_native);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode encrypt(Handle h_benchmark, Handle h_plaintext, Handle *h_ciphertext)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!h_benchmark.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_benchmark'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_plaintext.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_plaintext'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_ciphertext)
            throw HEBenchError(HEBERROR_MSG("Invalid null handle 'h_ciphertext'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BenchmarkHandle *p_bh = reinterpret_cast<BenchmarkHandle *>(h_benchmark.p);
        *h_ciphertext         = p_bh->p_benchmark->encrypt(h_plaintext);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode decrypt(Handle h_benchmark, Handle h_ciphertext, Handle *h_plaintext)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!h_benchmark.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_benchmark'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_ciphertext.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_ciphertext'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_plaintext)
            throw HEBenchError(HEBERROR_MSG("Invalid null handle 'h_plaintext'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BenchmarkHandle *p_bh = reinterpret_cast<BenchmarkHandle *>(h_benchmark.p);
        *h_plaintext          = p_bh->p_benchmark->decrypt(h_ciphertext);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode load(Handle h_benchmark,
               const Handle *h_local_packed_params, std::uint64_t local_count,
               Handle *h_remote_packed_params)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!h_benchmark.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_benchmark'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_local_packed_params)
            throw HEBenchError(HEBERROR_MSG("Invalid null array 'h_locals'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (local_count <= 0)
            throw HEBenchError(HEBERROR_MSG("Invalid empty array 'h_locals': 'local_count' must not be zero."),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_remote_packed_params)
            throw HEBenchError(HEBERROR_MSG("Invalid null handle 'h_remote'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BenchmarkHandle *p_bh   = reinterpret_cast<BenchmarkHandle *>(h_benchmark.p);
        *h_remote_packed_params = p_bh->p_benchmark->load(h_local_packed_params, local_count);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode store(Handle h_benchmark,
                Handle h_remote,
                Handle *h_local_packed_params, std::uint64_t local_count)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!h_benchmark.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_benchmark'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_remote.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_remote'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_local_packed_params)
            throw HEBenchError(HEBERROR_MSG("Invalid null argument 'h_local_packed_params'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BenchmarkHandle *p_bh = reinterpret_cast<BenchmarkHandle *>(h_benchmark.p);
        p_bh->p_benchmark->store(h_remote, h_local_packed_params, local_count);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

ErrorCode operate(Handle h_benchmark,
                  Handle h_remote_packed_params,
                  const ParameterIndexer *p_param_indexers,
                  uint64_t indexers_count,
                  Handle *h_remote_output)
{
    ErrorCode retval = HEBENCH_ECODE_SUCCESS;

    try
    {
        if (!h_benchmark.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_benchmark'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_remote_packed_params.p)
            throw HEBenchError(HEBERROR_MSG("Invalid empty handle 'h_remote_packed_params'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!p_param_indexers)
            throw HEBenchError(HEBERROR_MSG("Invalid null argument 'p_param_indexers'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);
        if (!h_remote_output)
            throw HEBenchError(HEBERROR_MSG("Invalid null argument 'h_remote_output'"),
                               HEBENCH_ECODE_CRITICAL_ERROR);

        BenchmarkHandle *p_bh = reinterpret_cast<BenchmarkHandle *>(h_benchmark.p);
        *h_remote_output      = p_bh->p_benchmark->operate(h_remote_packed_params, p_param_indexers, indexers_count);
    }
    catch (HEBenchError &hebench_err)
    {
        retval = hebench_err.getErrorCode();
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
    }

    return retval;
}

std::uint64_t getSchemeName(Handle h_engine, Scheme s, char *p_name, std::uint64_t size)
{
    std::uint64_t retval = 0;

    try
    {
        if (!checkHandleBits(h_engine, BaseEngine::tag))
            throw std::invalid_argument("h_engine");

        BaseEngine *p_engine = reinterpret_cast<BaseEngine *>(h_engine.p);
        auto it              = p_engine->schemeName().find(s);
        if (it == p_engine->schemeName().end())
            throw std::runtime_error("Scheme not found.");
        retval = Utilities::copyString(p_name, size, it->second);
    }
    catch (HEBenchError &hebench_err)
    {
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = 0;
    }

    return retval;
}

std::uint64_t getSchemeSecurityName(Handle h_engine, Scheme s, Security sec,
                                    char *p_name, std::uint64_t size)
{
    (void)s;
    std::uint64_t retval = 0;

    try
    {
        if (!checkHandleBits(h_engine, BaseEngine::tag))
            throw std::invalid_argument("h_engine");

        BaseEngine *p_engine = reinterpret_cast<BaseEngine *>(h_engine.p);
        auto it              = p_engine->securityName().find(sec);
        if (it == p_engine->securityName().end())
            throw std::runtime_error("Security not found.");
        retval = Utilities::copyString(p_name, size, it->second);
    }
    catch (HEBenchError &hebench_err)
    {
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = 0;
    }

    return retval;
}

std::uint64_t getBenchmarkDescriptionEx(Handle h_engine,
                                        Handle h_bench_desc,
                                        const hebench::APIBridge::WorkloadParams *p_w_params,
                                        char *p_description, std::uint64_t size)
{
    std::uint64_t retval = 0;

    try
    {
        if (!checkHandleBits(h_engine, BaseEngine::tag))
            throw std::invalid_argument("h_engine");

        BaseEngine *p_engine = reinterpret_cast<BaseEngine *>(h_engine.p);

        retval = Utilities::copyString(p_description, size,
                                       p_engine->getBenchmarkDescriptionEx(h_bench_desc, p_w_params));
    }
    catch (HEBenchError &hebench_err)
    {
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
        retval = 0;
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = 0;
    }

    return retval;
}

std::uint64_t getErrorDescription(ErrorCode code, char *p_description, std::uint64_t size)
{
    std::uint64_t retval = 0;

    try
    {
        retval = Utilities::copyString(p_description, size, BaseEngine::getErrorDesc(code));
    }
    catch (HEBenchError &hebench_err)
    {
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = 0;
    }

    return retval;
}

std::uint64_t getLastErrorDescription(Handle h_engine, char *p_description, std::uint64_t size)
{
    std::uint64_t retval = 0;

    try
    {
        (void)h_engine;
        retval = Utilities::copyString(p_description, size, BaseEngine::getLastErrorDesc());
    }
    catch (HEBenchError &hebench_err)
    {
        BaseEngine::setLastError(hebench_err.getErrorCode(), hebench_err.what());
        retval = 0;
    }
    catch (std::exception &ex)
    {
        retval = HEBENCH_ECODE_CRITICAL_ERROR;
        BaseEngine::setLastError(retval, ex.what());
    }
    catch (...)
    {
        retval = 0;
    }

    return retval;
}

} // namespace APIBridge
} // namespace hebench
