
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <cstring>
#include <memory>
#include <vector>

#include "../include/ex_benchmark.h"
#include "../include/ex_engine.h"

//-----------------------------------
// class ExampleBenchmarkDescription
//-----------------------------------

ExampleBenchmarkDescription::ExampleBenchmarkDescription()
{
    // initialize the descriptor for this benchmark
    std::memset(&m_descriptor, 0, sizeof(hebench::APIBridge::BenchmarkDescriptor));
    m_descriptor.workload                                   = hebench::APIBridge::Workload::MatrixMultiply;
    m_descriptor.data_type                                  = hebench::APIBridge::DataType::Float64;
    m_descriptor.category                                   = hebench::APIBridge::Category::Latency;
    m_descriptor.cat_params.min_test_time_ms                = 2000; // 2s
    m_descriptor.cat_params.latency.warmup_iterations_count = 1;
    m_descriptor.cipher_param_mask                          = HEBENCH_HE_PARAM_FLAGS_ALL_PLAIN;
    //
    m_descriptor.scheme   = HEBENCH_HE_SCHEME_PLAIN;
    m_descriptor.security = HEBENCH_HE_SECURITY_NONE;
    m_descriptor.other    = 0; // no extras needed for our purpose:
        // Other backends can use this field to differentiate between
        // benchmarks for which internal parameters, not specified by
        // other fields of this structure, differ.

    // specify default arguments for this workload:
    // this benchmark will only support matrices of 100x100
    hebench::cpp::WorkloadParams::MatrixMultiply default_workload_params;
    default_workload_params.rows_M0() = 100;
    default_workload_params.cols_M0() = 100;
    default_workload_params.cols_M1() = 100;
    this->addDefaultParameters(default_workload_params);
}

ExampleBenchmarkDescription::~ExampleBenchmarkDescription()
{
    // nothing needed in this example
}

hebench::cpp::BaseBenchmark *ExampleBenchmarkDescription::createBenchmark(hebench::cpp::BaseEngine &engine,
                                                                          const hebench::APIBridge::WorkloadParams *p_params)
{
    if (!p_params)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid empty workload parameters. This workload requires flexible parameters."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);

    ExampleEngine &ex_engine = dynamic_cast<ExampleEngine &>(engine);
    return new ExampleBenchmark(ex_engine, m_descriptor, *p_params);
}

void ExampleBenchmarkDescription::destroyBenchmark(hebench::cpp::BaseBenchmark *p_bench)
{
    // make sure we are destroying a benchmark object we created
    if (p_bench)
    {
        ExampleBenchmark *p_tmp = dynamic_cast<ExampleBenchmark *>(p_bench);
        delete p_tmp;
    } // end if
}

//------------------------
// class ExampleBenchmark
//------------------------

ExampleBenchmark::ExampleBenchmark(ExampleEngine &engine,
                                   const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                                   const hebench::APIBridge::WorkloadParams &bench_params) :
    hebench::cpp::BaseBenchmark(engine, bench_desc, bench_params)
{
    // validate workload parameters

    // number of workload parameters (3 for matmul: rows of M0, cols of M0, cols of M1)
    if (bench_params.count < ExampleBenchmarkDescription::NumWorkloadParams)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid workload parameters. This workload requires " + std::to_string(ExampleBenchmarkDescription::NumWorkloadParams) + " parameters."),
                                         HEBENCH_ECODE_INVALID_ARGS);

    // check values of the workload parameters and make sure they are supported by benchmark:

    hebench::cpp::WorkloadParams::MatrixMultiply w_params(bench_params);

    if (w_params.rows_M0() != 100
        || w_params.cols_M0() != 100
        || w_params.cols_M1() != 100)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid workload parameters. This workload only supports matrices of dimensions 100 x 100."),
                                         HEBENCH_ECODE_INVALID_ARGS);

    // workload-parameter-based initialization would go here, but for this example is
    // not necessary because this example supports only matrices that are 100 x 100
}

ExampleBenchmark::~ExampleBenchmark()
{
    // nothing needed in this example
}

hebench::APIBridge::Handle ExampleBenchmark::encode(const hebench::APIBridge::DataPackCollection *p_parameters)
{
    if (p_parameters->pack_count != ParametersCount)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid number of parameters detected in parameter pack. Expected 2."),
                                         HEBENCH_ECODE_INVALID_ARGS);

    // allocate our internal version of the encoded data

    // We are using shared_ptr because we want to be able to copy the pointer object later
    // and use the reference counter to avoid leaving dangling. If our internal object
    // does not need to be copied, shared_ptr is not really needed.
    std::shared_ptr<std::vector<Matrix>> p_params =
        std::make_shared<std::vector<Matrix>>(p_parameters->pack_count);
    std::vector<Matrix> &params = *p_params;

    // encode the packed parameters into our internal version
    for (std::uint64_t param_i = 0; param_i < p_parameters->pack_count; ++param_i)
    {
        // find the parameter data pack inside the parameters pack that corresponds
        // to this parameter position:
        // param_i == p_parameters->p_data_packs[i].param_position
        const hebench::APIBridge::DataPack &parameter = ExampleBenchmark::findDataPack(*p_parameters, param_i);
        // take first sample from parameter (because latency test has a single sample per parameter)
        if (!parameter.p_buffers || !parameter.p_buffers[0].p)
            throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid empty samples detected in parameter pack."),
                                             HEBENCH_ECODE_INVALID_ARGS);
        const hebench::APIBridge::NativeDataBuffer &sample = parameter.p_buffers[0];
        // convert the native data to pointer to double as per specification of workload
        const double *p_row = reinterpret_cast<const double *>(sample.p);

        // copy every 100 doubles (full row) to each row of the matrix representation

        // We cannot just simply maintain pointers to the parameter data because, as per specification,
        // the resulting handle must be valid regardless whether the native data is valid after
        // this method completes. Thus, deep copy is needed.
        for (std::size_t row_i = 0; row_i < params[param_i].rows.size(); ++row_i)
        {
            std::copy(p_row, p_row + params[param_i].rows[row_i].size(),
                      params[param_i].rows[row_i].data());
            p_row += params[param_i].rows[row_i].size();
        } // end for
    } // end for

    // wrap our internal object into a handle to cross the boundary of the API Bridge
    return this->getEngine().template createHandle<decltype(p_params)>(sizeof(Matrix) * params.size(), 0,
                                                                       p_params);
}

void ExampleBenchmark::decode(hebench::APIBridge::Handle encoded_data, hebench::APIBridge::DataPackCollection *p_native)
{
    // This method should handle decoding of data encoded using encode(), due to
    // specification stating that encode() and decode() are inverses; as well as
    // handle data decrypted from operation() results.

    // retrieve our internal format object from the handle
    const std::vector<Matrix> &params =
        *this->getEngine().template retrieveFromHandle<std::shared_ptr<std::vector<Matrix>>>(encoded_data);

    // according to specification, we must decode as much data as possible, where
    // any excess encoded data that won't fit into the pre-allocated native buffer
    // shall be ignored

    std::uint64_t min_param_count = std::min(p_native->pack_count, params.size());
    for (std::size_t param_i = 0; param_i < min_param_count; ++param_i)
    {
        hebench::APIBridge::DataPack *p_native_param = &p_native->p_data_packs[param_i];

        if (p_native_param && p_native_param->buffer_count > 0)
        {
            // for latency, we have only one sample, so, decode the sample into the first buffer
            hebench::APIBridge::NativeDataBuffer &native_sample = p_native_param->p_buffers[0];

            // copy each row for the current parameter matrix into the corresponding
            // decoded buffer

            const Matrix &mat = params[param_i]; // alias for clarity

            std::uint64_t sample_elem_count = // number of doubles in decoded buffer
                native_sample.size / sizeof(double);
            std::uint64_t offset = 0;
            for (std::size_t row_i = 0;
                 offset < sample_elem_count
                 && row_i < mat.rows.size();
                 ++row_i)
            {
                // point to next row in target
                double *p_decoded_row = reinterpret_cast<double *>(native_sample.p) + offset;

                // copy as much as we can into the row
                std::uint64_t num_elems_to_copy =
                    std::min(mat.rows[row_i].size(), sample_elem_count - offset);
                std::copy(mat.rows[row_i].data(), mat.rows[row_i].data() + num_elems_to_copy,
                          p_decoded_row);

                offset += num_elems_to_copy; // advance the target row pointer
            } // end for
        } // end if
    } // end for
}

hebench::APIBridge::Handle ExampleBenchmark::encrypt(hebench::APIBridge::Handle encoded_data)
{
    // we only do plain text in this example, so, just return a copy of our internal data
    std::shared_ptr<void> p_encoded_data =
        this->getEngine().template retrieveFromHandle<std::shared_ptr<void>>(encoded_data);

    // the copy is shallow, but shared_ptr ensures correct destruction using reference counting
    return this->getEngine().template createHandle<decltype(p_encoded_data)>(encoded_data.size, 0,
                                                                             p_encoded_data);
}

hebench::APIBridge::Handle ExampleBenchmark::decrypt(hebench::APIBridge::Handle encrypted_data)
{
    // we only do plain text in this example, so, just return a copy of our internal data
    std::shared_ptr<void> p_encrypted_data =
        this->getEngine().template retrieveFromHandle<std::shared_ptr<void>>(encrypted_data);

    // the copy is shallow, but shared_ptr ensures correct destruction using reference counting
    return this->getEngine().template createHandle<decltype(p_encrypted_data)>(encrypted_data.size, 0,
                                                                               p_encrypted_data);
}
hebench::APIBridge::Handle ExampleBenchmark::load(const hebench::APIBridge::Handle *p_local_data, uint64_t count)
{
    if (count != 1)
        // we do all ops in plain text, so, we should get only one pack of data
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid number of handles. Expected 1."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    if (!p_local_data)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid null array of handles: \"p_local_data\""),
                                         HEBENCH_ECODE_INVALID_ARGS);

    // since remote and host are the same for this example, we just need to return a copy
    // of the local data as remote.

    return this->getEngine().duplicateHandle(p_local_data[0]);
}

void ExampleBenchmark::store(hebench::APIBridge::Handle remote_data,
                             hebench::APIBridge::Handle *p_local_data, std::uint64_t count)
{
    if (count > 0 && !p_local_data)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid null array of handles: \"p_local_data\""),
                                         HEBENCH_ECODE_INVALID_ARGS);

    if (count > 0)
    {
        // pad with zeros any remaining local handles as per specifications
        std::memset(p_local_data, 0, sizeof(hebench::APIBridge::Handle) * count);

        // since remote and host are the same for this example, we just need to return a copy
        // of the remote as local data.

        p_local_data[0] = this->getEngine().duplicateHandle(remote_data);
    } // end if
}

hebench::APIBridge::Handle ExampleBenchmark::operate(hebench::APIBridge::Handle h_remote_packed,
                                                     const hebench::APIBridge::ParameterIndexer *p_param_indexers)
{
    // This method should perform as fast as possible since it is the
    // method benchmarked by Test Harness.

    for (std::size_t i = 0; i < 2; ++i)
        // normally, a robust backend will use the indexers as appropriate,
        // but for the sake of the example, we just validate them
        if (p_param_indexers[i].value_index != 0 || p_param_indexers[i].batch_size != 1)
            throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid parameter indexer. Expected index 0 and batch size of 1."),
                                             HEBENCH_ECODE_INVALID_ARGS);

    // retrieve our internal format object from the handle
    const std::vector<Matrix> &params =
        *this->getEngine().template retrieveFromHandle<std::shared_ptr<std::vector<Matrix>>>(h_remote_packed);

    // create a new internal object for result
    std::uint64_t components_count                = ResultComponentsCount;
    std::shared_ptr<std::vector<Matrix>> p_result = std::make_shared<std::vector<Matrix>>(components_count);

    // perform the actual operation
    Matrix &result = p_result->front(); // alias the pointer for clarity
    for (std::size_t row_0_i = 0; row_0_i < params[0].rows.size(); ++row_0_i)
    {
        for (std::size_t col_0_i = 0; col_0_i < params[0].rows[row_0_i].size(); ++col_0_i)
        {
            double val = 0;
            for (int i = 0; i < 100; i++)
                val += params[0].rows[row_0_i][i] * params[1].rows[i][col_0_i];
            result.rows[row_0_i][col_0_i] = val;
        } // end for
    } // end for

    // send our internal result across the boundary of the API Bridge as a handle
    return this->getEngine().template createHandle<decltype(p_result)>(sizeof(Matrix) * p_result->size(), 0,
                                                                       p_result);
}
