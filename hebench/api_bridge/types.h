
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_Types_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_Types_H_7e5fa8c2415240ea93eff148ed73539b

#include <cstdint>

#include "version.h"

namespace hebench {
namespace APIBridge {

//================
// Error handling
//================

/**
 * @brief Return value for API bridge functions.
 * @details A value of `HEBENCH_ECODE_SUCCESS` means success.
 *
 * An error value of `ECODE_CRITICAL_ERROR` will prompt the Test Harness to terminate.
 * This usually indicates an unrecoverable error that indicates that the backend engine
 * encounter an unstable state and it affects the backend engine capacity to recover,
 * cleanup and/or create new benchmarks. On this error, Test Harness will stop benchmarking
 * and attempt to clean up any open handles before exiting. Any further errors are ignored.
 *
 * Backend should custom-define and return appropriate error codes for all required
 * operations through the API bridge. Custom error codes values must be different than
 * those pre-defined in the API bridge.
 * @sa getErrorDescription()
 */
typedef std::int32_t ErrorCode;

// Predefined error codes

//! Function call succeeded without error.
#define HEBENCH_ECODE_SUCCESS 0

//! Indicates invalid arguments to function call.
#define HEBENCH_ECODE_INVALID_ARGS 0x7ffffffe

//! Specifies a critical error.
/*!
Returning this on an API bridge call will prompt the Test Harness to terminate immediately.
Test Harness will stop benchmarking and attempt to clean up any open handles before
exiting. Any further errors are ignored.
*/
#define HEBENCH_ECODE_CRITICAL_ERROR 0x7fffffff

//===============
// Miscellaneous
//===============

#define HEBENCH_MAX_BUFFER_SIZE 256

//=======================
// Benchmark descriptors
//=======================

//! Maximum number of parameters supported by an operation.
#define HEBENCH_MAX_OP_PARAMS 32
//! Maximum number of parameters for Category benchmark descriptor.
#define HEBENCH_MAX_CATEGORY_PARAMS HEBENCH_MAX_OP_PARAMS * 2

// Sugested common scheme types
#define HEBENCH_HE_SCHEME_CKKS 100
#define HEBENCH_HE_SCHEME_BFV  101
#define HEBENCH_HE_SCHEME_BGV  102

// common flags for parameter combinations

#define HEBENCH_HE_PARAM_FLAGS_ALL_CIPHER 0xffffffff
#define HEBENCH_HE_PARAM_FLAGS_ALL_PLAIN  0x0

/**
 * @brief Defines all possible workloads.
 * @details See extended documentation for detailed definition, description,
 * data layout, parameters, results, etc.
 */
enum Workload
{
    /*! Matrix Multiplication workload.
     *
     * Workload Params:
     * - 0: `uint64` - number of rows in matrix `M0`: `rows0`.
     * - 1: `uint64` - number of columns in matrix `M0`: `cols0`.
     * - 2: `uint64` - number of columns in matrix `M1`: `cols1`.
     *
     * Operation Params: `M0`, `M1`
     *
     * - 0: `M0` - matrix with `rows0` rows and `cols0` columns.
     * - 1: `M1` - matrix with `cols0` rows and `cols1` columns.
     *
     * Result:
     *
     * - 0: `M` - matrix with `rows0` rows and `cols1` columns.
     * @code
     * M = M0 x M1
     * @endcode
     *
     * For details see \ref matrix_multiplication .*/
    MatrixMultiply,
    /*! Vector Element-wise Addition workload.
     *
     * Workload Params:
     * - 0: `uint64` - number of elements in vector: `n`.
     *
     * Operation Params: `A`, `B`
     *
     * - 0: `A` - vector with `n` contiguous elements in memory.
     * - 1: `B` - vector with `n` contiguous elements in memory.
     *
     * Result:
     *
     * - 0: `C` - vector with `n` contiguous elements in memory.
     * @code
     * C[i] = A[i] + B[i]
     * @endcode
     *
     * For details see \ref elementwise_add .*/
    EltwiseAdd,
    /*! Vector Element-wise Multiplication workload.
     *
     * Workload Params:
     * - 0: `uint64` - number of elements in vector: `n`.
     *
     * Operation Params: `A`, `B`
     *
     * - 0: `A` - vector with `n` contiguous elements in memory.
     * - 1: `B` - vector with `n` contiguous elements in memory.
     *
     * Result:
     *
     * - 0: `C` - vector with `n` contiguous elements in memory.
     * @code
     * C[i] = A[i] * B[i]
     * @endcode
     *
     * For details see \ref elementwise_mult .*/
    EltwiseMultiply,
    /*! Vector Dot Product workload.
     *
     * Workload Params:
     * - 0: `uint64` - number of elements in vector: `n`.
     *
     * Operation Params: `A`, `B`
     *
     * - 0: `A` - vector with `n` contiguous elements in memory.
     * - 1: `B` - vector with `n` contiguous elements in memory.
     *
     * Result:
     *
     * - 0: `c` - scalar.
     * @code
     * c = A[0] * B[0] + A[1] * B[1] + ... + A[n - 1] * B[n - 1]
     * @endcode
     *
     * For details see \ref dot_product .*/
    DotProduct,
    /*! Logistic Regression Inference workload.
     *
     * Workload Params:
     * - 0: `uint64` - `n`: number of features in feature vector.
     *
     * Operation Params: `W`, `b`, `X`
     *
     * - 0: `W` - feature vector.
     * - 1: `b` - scalar bias.
     * - 2: `X` - input vector with `n` elements.
     *
     * Result:
     *
     * - 0: P(X) - scalar.
     * @code
     * P(X) = sigmoid(W . X + b)
     * @endcode
     *
     * For details see \ref logistic_regression .*/
    LogisticRegression,
    /*! Logistic Regression Inference workload with polynomial approximation.
     *
     * Workload Params:
     * - 0: `uint64` - `n`: number of features in feature vector.
     *
     * Operation Params: `W`, `b`, `X`
     *
     * - 0: `W` - feature vector.
     * - 1: `b` - scalar bias.
     * - 2: `X` - input vector with `n` elements.
     *
     * Result:
     *
     * - 0: P(X) - scalar.
     * @code
     * P(X) = sigmoid(W . X + b)
     * @endcode
     * where `sigmoid_pd3` is the sigmoid function computed using
     * polynomial approximation of degree `3` around `x = 0`.
     *
     * For details see \ref logistic_regression .*/
    LogisticRegression_PolyD3,
    /*! Logistic Regression Inference workload with polynomial approximation.
     *
     * Workload Params:
     * - 0: `uint64` - `n`: number of features in feature vector.
     *
     * Operation Params: `W`, `b`, `X`
     *
     * - 0: `W` - feature vector.
     * - 1: `b` - scalar bias.
     * - 2: `X` - input vector with `n` elements.
     *
     * Result:
     *
     * - 0: P(X) - scalar.
     * @code
     * P(X) = sigmoid(W . X + b)
     * @endcode
     * where `sigmoid_pd5` is the sigmoid function computed using
     * polynomial approximation of degree `5` around `x = 0`.
     *
     * For details see \ref logistic_regression .*/
    LogisticRegression_PolyD5,
    /*! Logistic Regression Inference workload with polynomial approximation.
     *
     * Workload Params:
     * - 0: `uint64` - `n`: number of features in feature vector.
     *
     * Operation Params: `W`, `b`, `X`
     *
     * - 0: `W` - feature vector.
     * - 1: `b` - scalar bias.
     * - 2: `X` - input vector with `n` elements.
     *
     * Result:
     *
     * - 0: P(X) - scalar.
     * @code
     * P(X) = sigmoid(W . X + b)
     * @endcode
     * where `sigmoid_pd7` is the sigmoid function computed using
     * polynomial approximation of degree `7` around `x = 0`.
     *
     * For details see \ref logistic_regression .*/
    LogisticRegression_PolyD7
};

namespace WorkloadParamType {

/**
 * @brief Defines the possible data types for a workload flexible parameter.
 */
enum WorkloadParamType
{
    Int64, //!< 64 bits signed integers.
    UInt64, //!< 64 bits unsigned integers.
    Float64 //!< 64 bits IEEE 754 standard floating point real numbers.
};

} // namespace WorkloadParamType

/**
 * @brief Defines a single workload parameter.
 * @details This struct holds the value and meta data for a workload parameter.
 * The correct type is workload specific and specified per workload.
 * @sa WorkloadParams
 */
struct WorkloadParam
{
    /**
     * @brief Type of the parameter data.
     */
    WorkloadParamType::WorkloadParamType data_type;
    /**
     * @brief Null-terminated string containing the name for the parameter.
     * @details This name if for description purposes only and should not be used
     * as identifier or any other specific purpose.
     */
    char name[HEBENCH_MAX_BUFFER_SIZE];
    union
    {
        /**
         * @brief 64 bits signed integer argument value for workload parameter.
         * @details Only one of `i_param`, `u_param`, `f_param` must be set
         * for the workload parameter, based on `data_type`.
         */
        int64_t i_param;
        /**
         * @brief 64 bits unsigned integer argument value for workload parameter.
         * @details Only one of `i_param`, `u_param`, `f_param` must be set
         * for the workload parameter, based on `data_type`.
         */
        uint64_t u_param;
        /**
         * @brief 64 bits double precision floating point argument value for
         * workload parameter.
         * @details Only one of `i_param`, `u_param`, `f_param` must be set
         * for the workload parameter, based on `data_type`.
         */
        double f_param;
    };
};

/**
 * @brief Specifies the parameters for a workload.
 * @details
 * A workload can have `0` or more flexible parameters that
 * can be specified during benchmark initialization; for example, vector element-wise
 * operations have the number of elements in a vector as a parameter.
 *
 * The number of parameters is workload specific.
 */
struct WorkloadParams
{
    /**
     * @brief Parameters for the workload.
     */
    WorkloadParam *params;
    /**
     * @brief Number of workload parameters.
     */
    std::uint64_t count;
};

/**
 * @brief Defines data types for a workload.
 * @details A workload may not support all data types.
 */
enum DataType
{
    Int32, //!< 32 bits signed integers.
    Int64, //!< 64 bits signed integers.
    Float32, //!< 32 bits IEEE 754 standard floating point real numbers.
    Float64 //!< 64 bits IEEE 754 standard floating point real numbers.
};

/**
 * @brief Defines all possible categories for each workload.
 * @details Note that some workloads may not support certain categories.
 */
enum Category
{
    /*! Test Harness sends the same single data sample repeatedly to backend. */
    Latency,
    /*! Test Harness loads the whole dataset to backend before requesting all the results in a single operation.<br>
     * For multiple samples on each input, the results are ordered in a row-major fashion.<br>
     * For more information, see \ref results_order .
     */
    Offline
};

/**
 * @brief Open-ended homomorphic encryption scheme ID.
 * @details Name and details of the scheme can be obtained using helper
 * functions from the API bridge.
 */
typedef std::int32_t Scheme;
/**
 * @brief Open-ended homomorphic encryption scheme security ID.
 * @details Name and details of the scheme security can be obtained using
 * helper functions from the API bridge.
 */
typedef std::int32_t Security;

/**
 * @brief Specifies parameters for a category.
 * @details This union is flexible to accomodate parameters for all categories.
 * Clients must be mindful that writing data to parameters for a category
 * will overwrite data for all other categories in the same union, thus,
 * separate instaces of this type should be used for different categories.
 */
union CategoryParams
{
    /**
     * @brief Generic placeholder for categories.
     * @details This field is used to allocate memory for the whole union.
     * It is advised to access the parameters through the respective category
     * field instead.
     */
    std::uint64_t reserved[HEBENCH_MAX_CATEGORY_PARAMS];
    struct
    {
        /**
         * @brief Specifies the minimum time, in milliseconds, to test for latency.
         * @details Latency benchmark will submit an operation with the same set
         * of inputs as many times as needed until the time elapsed during the test
         * is, at least, the number of milliseconds specified. A value of 0, indicates
         * to Test Harness to use default latency test time.
         *
         * Test Harness will submit, at least, two iterations regardless the time
         * specified.
         */
        std::uint64_t min_test_time_ms;
        /**
         * @brief Specifies the number of warmup iterations to perform for latency
         * test.
         * @details Latency benchmark will run the operation this many iterations with
         * the same set of inputs before doing any actual measurement. The operation
         * results and measured times for the operation during warmup are not counted
         * under the actual operation timing, although may be reported under warmup.
         */
        std::uint64_t warmup_iterations_count;
    } latency; //!< Specifies the parameters for the latency category.
    struct
    {
        /**
         * @brief Specifies the number of data samples for each parameter for
         * Category::Offline.
         * @details This specifies the the number of samples submitted by Test Harness
         * for each parameter for the offline operation.
         *
         * A value larger than available input samples will see the whole
         * dataset of inputs be used for that parameter in a single operation. Note that
         * in case that there is not enough data samples for a parameter, the number
         * of samples will be lower than requested.
         *
         * A value of 0 indicates to Test Harness that number of samples for the
         * corresponding operation parameter accepts any value. In this case, sample
         * size is specified by Test Harness user options. If value specified by Test
         * harness is `0`, the sample size for the operation parameter is defined in
         * the workload specification.
         *
         * Regardless of the requested sample sizes (specified or default), the actual
         * number of samples may be limited by the dataset for the workload, if any.
         */
        std::uint64_t data_count[HEBENCH_MAX_OP_PARAMS];
    } offline; //!< Specifies the parameters for the offline category.
};

/**
 * @brief Defines a benchmark test.
 * @details A benchmark test is defined by a Workload, a Category for said workload,
 * a scheme for such Category, which inputs are plain text of cipher text,
 * the security level for the scheme, and an extra parameter that is backend
 * specific.
 *
 * `cipher_param_mask` is a bit mask that defines which operation parameters are cipher
 * or plain text. If bit `i` is set, then parameter `i` is cipher text, otherwise,
 * it is plain text. Only the least significant `n` bits of this field will be used
 * for an operation that supports `n` operands. The rest are ignored.
 *
 * Test Harness will request the list of BenchmarkDescriptor objects for which a backend
 * is subscribing. When Test Harness is about to execute a benchmark, the BenchmarkDescriptor
 * is passed down to the backend with the same values used during subscription.
 *
 * Fields `workload`, `data_type`, `category`, `cat_params` and `cypher_param_mask`
 * specify the actual operation for the benchmark that Test Harness will execute.
 * Different combination of values specified by `scheme`, `security`, and `other`
 * fields do not affect the type of benchmark, but further describe it.
 *
 * A benchmark is defined by the combination of the values for all these fields, thus,
 * two benchmarks are considered the same if all fields have the same value. So, if
 * we are running two benchmark with workload matrix multiplication, latency category,
 * CKKS scheme with 128 bits security and BFV scheme with 128 bits security,
 * then scheme field for both benchmarks must be different. Note that two benchmarks
 * with the same descriptor can differ in the workload parameters, effectively
 * performing the same benchmark on a workload with different parameter sets.
 * @sa WorkloadParams
 */
struct BenchmarkDescriptor
{
    // benchmark specification
    Workload workload; //!< Workload for the benchmark.
    DataType data_type; //!< Data type for the workload.
    Category category; //!< Category for the benchmark.
    CategoryParams cat_params; //!< Parameters for the category.
    std::uint32_t cipher_param_mask; //!< Input mask to define which operation parameters for the computation are plain text or cipher text.
    // extra description
    Scheme scheme; //!< Scheme for the benchmark.
    Security security; //!< Security for the scheme.
    std::int64_t other; //!< Backend specific extra parameter.
};

//================================
// Data movement and organization
//================================

/**
 * @brief Structure to contain flexible data.
 * @details What all of this structure's fields actually represent, is custom-defined
 * by the structure user. Field names are just provided as a guideline. It is the
 * client's responsibility to define the meaning of each field based on where is this
 * structure used and to adhere to this definition.
 */
struct _FlexibleData
{
    //! Pointer to underlying data.
    /*!
    While this field does not necessarily need to point to any data (or contain a
    valid pointer value), it usually points to underlying data buffer(s).
    */
    void *p;
    //! Size of underlying data.
    /*!
    Usually specifies the size, in bytes, of the underlying data represented by this
    structure.
    */
    std::uint64_t size;
    //! Optional tag.
    /*!
    Flexible extra field to allow further customization of this structure.
    */
    std::int64_t tag;
};

#define NULL_HANDLE hebench::APIBridge::_FlexibleData({ 0, 0, 0 })

/**
 * @brief Represents a native data buffer maintained by the Test Harness.
 * @details This structure is created and maintained by the Test Harness and
 * is passed to the backend through the API bridge.
 *
 * The buffer pointed to by the `p` field is formatted to contain data in
 * the format expected for the respective parameter for which the buffer
 * is used, and thus, it is safe to cast to the propper data type pointer.
 *
 * Note that the `size` field contains the size in bytes of the buffer, not
 * the number of items of the corresponding data type.
 *
 * The `tag` field should not be modified outside of the Test Harness.
 */
typedef _FlexibleData NativeDataBuffer;
/**
 * @brief Encapsulates an opaque handle to backend data.
 * @details All fields of this struct are managed by the backend. Test
 * harness will not modify any of them, unless otherwise specified. It
 * is up to the backend to set these fields to match its needs.
 */
typedef _FlexibleData Handle;

/**
 * @brief Defines a data package for an operation.
 * @details Field `param_position` specifies the zero-based position of the
 * data as a parameter in the call order for the operation to execute.
 * For example, in op(A, B, C), A is at position 0, B is at position 1,
 * C is at 2, and so on.
 *
 * This data pack contains a list of samples for the same parameter which
 * will be addressed by index during the call to the operation.
 */
struct DataPack
{
    NativeDataBuffer *p_buffers; //!< Array of data buffers for parameter.
    std::uint64_t buffer_count; //!< Number of data buffers in `p_buffers`.
    std::uint64_t param_position; //!< The 0-based position of this parameter in the corresponding function call.
};

/**
 * @brief Defines a collection of data packs.
 * @details This struct is used to encode/decode a collection of data packs,
 * in a single call instead of encoding each data pack at a time. This
 * is useful for backends to optimize large amounts of data.
 */
struct PackedData
{
    DataPack *p_data_packs; //!< Collection of data packs.
    std::uint64_t pack_count; //!< Number of data packs in the collection.
};

/**
 * @brief Indexes a DataPack.
 * @details In a call to operate(), a list of `ParameterIndexer` is passed,
 * where each element of that list corresponds to an operation parameter position
 * in the call order. For that call to `operate()`, the `i`-th `ParameterIndexer`
 * specifies the index for the value to use from the corresponding `DataPack` and
 * how many values in the batch to use for the operation.
 *
 * For details on the ordering of results based on the operation parameter sample
 * index see \ref results_order .
 * @sa DataPack, operate(), \ref results_order
 */
struct ParameterIndexer
{
    std::uint64_t value_index; //!< Index of parameter value inside the data pack.
    std::uint64_t batch_size; //!< Number of values to use, starting from index.
};

} // namespace APIBridge
} // namespace hebench

#endif // defined _HEBench_API_Bridge_Types_H_7e5fa8c2415240ea93eff148ed73539b
