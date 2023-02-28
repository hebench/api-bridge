
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "../include/ex_engine.h"
#include "../include/ex_version.h"
#include <cstring>

// include all benchmarks
#include "../include/ex_benchmark.h"

//-----------------
// Engine creation
//-----------------

namespace hebench {
namespace cpp {

BaseEngine *createEngine(const std::int8_t *p_buffer, std::uint64_t size)
{
    (void)p_buffer; // not used in this example
    (void)size;

    if (HEBENCH_API_VERSION_MAJOR != HEBENCH_API_VERSION_NEEDED_MAJOR
        || HEBENCH_API_VERSION_MINOR != HEBENCH_API_VERSION_NEEDED_MINOR
        || HEBENCH_API_VERSION_REVISION < HEBENCH_API_VERSION_NEEDED_REVISION
        //|| std::strcmp(HEBENCH_API_VERSION_BUILD, HEBENCH_API_VERSION_NEEDED_BUILD) != 0
    )
        throw HEBenchError(HEBERROR_MSG("Critical: Invalid HEBench API version detected."),
                           HEBENCH_ECODE_CRITICAL_ERROR);

    return ExampleEngine::create();
}

void destroyEngine(BaseEngine *p)
{
    ExampleEngine *_p = dynamic_cast<ExampleEngine *>(p);
    ExampleEngine::destroy(_p);
}

} // namespace cpp
} // namespace hebench

//---------------------
// class ExampleEngine
//---------------------

ExampleEngine *ExampleEngine::create()
{
    ExampleEngine *p_retval = new ExampleEngine();
    p_retval->init();
    return p_retval;
}

void ExampleEngine::destroy(ExampleEngine *p)
{
    if (p)
        delete p;
}

ExampleEngine::ExampleEngine()
{
}

ExampleEngine::~ExampleEngine()
{
}

void ExampleEngine::init()
{
    // add any new error codes: use

    // this->addErrorCode(code, "generic description");

    // add supported schemes

    addSchemeName(HEBENCH_HE_SCHEME_PLAIN, "Plain");

    // add supported security

    addSecurityName(HEBENCH_HE_SECURITY_NONE, "None");

    // add the all benchmark descriptors
    addBenchmarkDescription(std::make_shared<ExampleBenchmarkDescription>());
}
