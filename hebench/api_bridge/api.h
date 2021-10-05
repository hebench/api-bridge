
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_H_7e5fa8c2415240ea93eff148ed73539b

#include "types.h"

namespace hebench {
namespace APIBridge {

/**
 * @brief Releases resources held by the specified handle.
 * @param[in] h Handle to destroy.
 * @return Error code.
 * @details The specified handle is no longer valid for any calls requiring a
 * handle after this function is called. This function releases resources held
 * by the handle in the back-end and destroys the handle.
 *
 * Test harness will call this method to clean up a handle it no longer needs.
 * Test harness will take care to destroy handles in reverse order in which it
 * obtained them.
 */
extern "C" ErrorCode destroyHandle(Handle h);

/**
 * @brief Initializes the back-end engine.
 * @param[out] h_engine Points to a handle to fill with the initialized
 * back-end engine handle descriptor. Must not be null.
 * @return Error code.
 * @details Use this method to initialize back-end engine, such as drivers,
 * hardware, etc. required to perform operations.
 */
extern "C" ErrorCode initEngine(Handle *h_engine);

/**
 * @brief Retrieves the number of benchmarks for which the back-end is registering
 * to perform.
 * @param[in] h_engine Back-end engine handle.
 * @param[out] p_count Points to variable to receive the result. Cannot be null.
 * @return Error code.
 * @details The number of benchmarks returned in \p p_count for which back-end is
 * subscribing must be greater than 0.
 */
extern "C" ErrorCode subscribeBenchmarksCount(Handle h_engine, std::uint64_t *p_count);
/**
 * @brief Retrieves handles to the benchmark descriptions for which the back-end is
 * registering to perform.
 * @param[in] h_engine Back-end engine handle.
 * @param[out] p_h_bench_descs Pointer to array with capacity for as many handles
 * as specified by subscribeBenchmarksCount(). Cannot be null.
 * @return Error code.
 * @details The handles in \p p_h_bench_descs are pointed to the internal representation
 * of the description of each benchmark that the back-end is registering to perform.
 * These benchmark description handles shall be used to query information about the
 * registered benchmarks without having to instantiate the represented benchmark proper,
 * and to instantiate the benchmark once it is time to execute it by calling the
 * appropriate API bridge functions.
 *
 * @sa subscribeBenchmarksCount()
 */
extern "C" ErrorCode subscribeBenchmarks(Handle h_engine, Handle *p_h_bench_descs);
/**
 * @brief Retrieves details about the flexible parameters supported by this workload.
 * @param[in] h_engine Handle to the back-end engine to perform the benchmark.
 * @param[in] h_bench_desc Handle to the benchmark description to query for information.
 * @param[out] p_param_count Points to variable to receive the number of workload parameters
 * required by the back-end.
 * @param[out] p_default_count Points to variable to receive the number of sets of default
 * workload parameters supported by the back-end.
 * @return Error code.
 * @details Value returned in \p p_param_count is 0 if and only if \p p_default_count
 * is also 0.
 * @sa describeBenchmark()
 */
extern "C" ErrorCode getWorkloadParamsDetails(Handle h_engine,
                                              Handle h_bench_desc,
                                              std::uint64_t *p_param_count,
                                              std::uint64_t *p_default_count);
/**
 * @brief Retrieves the concrete description for a benchmark registered by back-end.
 * @param[in] h_engine Handle to the back-end engine to perform the benchmark.
 * @param[in] h_bench_desc Handle to the benchmark description to query for information.
 * @param[out] p_bench_desc Points to the benchmark descriptor to receive the description
 * of the benchmark.
 * @param[out] p_default_params Array of `WorkloadParams` that will receive the sets of
 * default parameters supported by this workload. It can be null if caller does not need
 * this information.
 * @return Error code.
 * @details The array \p p_default_params is null if caller does not need the information
 * regarding default arguments for workload, otherwise, it must have enough capacity to
 * hold, at least, as many default sets as returned by getWorkloadParamsDetails().
 * If workload does not support parameters, \p p_default_params is ignored.
 *
 * @sa getWorkloadParamsDetails()
 */
extern "C" ErrorCode describeBenchmark(Handle h_engine,
                                       Handle h_bench_desc,
                                       BenchmarkDescriptor *p_bench_desc,
                                       WorkloadParams *p_default_params);
/**
 * @brief Instantiates a benchmark on the back-end.
 * @param[in] h_engine Handle to the back-end engine to perform the benchmark.
 * @param[in] h_bench_desc Handle to the description of the benchmark to instantiate.
 * @param[in] p_params A set of parameters for the benchmark to be instantiated.
 * It is ignored if workload does not support parameters.
 * @param[out] h_benchmark Points to a handle to be filled with the initialized benchmark.
 * @return Error code.
 * @details The returned benchmark handle will be used to reference this benchmark
 * while executing the benchmark operations by the Test harness. Destroying or modifying
 * \p h_bench_desc or \p p_params must not affect the benchmark instantiation.
 *
 * Back-end can assume that only a single benchmark handle will be initialized at
 * all times for the specified back-end engine.
 *
 * Parameter \p h_bench_desc is a handle to a description previously registered by
 * the back-end during the call to subscribeBenchmarks() of the benchmark to be
 * performed.
 *
 * If workload for the benchmark supports parameters \p p_params cannot be null;
 * otherwise, it is ignored and can be set to null.
 *
 * @sa subscribeBenchmarks()
 */
extern "C" ErrorCode initBenchmark(Handle h_engine,
                                   Handle h_bench_desc,
                                   const WorkloadParams *p_params,
                                   Handle *h_benchmark);

/**
 * @brief Given a pack of parameters in raw, native data format, encodes them into
 * plain text suitable for back-end encryption or operation.
 * @param[in] h_benchmark Handle to the initialized benchmark to perform.
 * @param[in] p_parameters Points to a PackedData instance containing the
 * information for the pack of parameters to encode.
 * @param[out] h_plaintext Opaque handle representing the resulting encoded data
 * by the back-end.
 * @return Error code.
 * @details The size and shape of each parameter are determined by the
 * Test harness based on expected input shapes of the operation performed. These
 * shapes are operation-dependent and are documented for each operation. It is the
 * responsibility of the back-end to keep track of these shapes (through the opaque
 * handle) so that they can be retrieved appropriately when needed.
 *
 * Unless otherwise specified by a workload or category, in general, Test harness will
 * encode all parameters that ought to be encrypted first in a single call to encode,
 * and all the plain text in another call.
 *
 * Destroying and cleaning up \p p_parameters after this call completes shall
 * not affect the resulting encoding.
 *
 * This function is the inverse of decode(), and thus, decoding the resulting
 * \p h_plaintext should result in the exact same raw, native data passed during encode.
 * @sa decode()
 */
extern "C" ErrorCode encode(Handle h_benchmark,
                            const PackedData *p_parameters,
                            Handle *h_plaintext);

/**
 * @brief Decodes plaintext data into the appropriate raw, native format.
 * @param[in] h_benchmark Handle to the initialized benchmark to perform.
 * @param[in] h_plaintext Handle to encoded data.
 * @param[out] p_native Pre-allocated buffers to contain the decoded output.
 * @return Error code.
 * @details The data buffers in \p p_native will be passed into this function
 * pre-allocated. The `param_position` field will be already set to the required
 * component for each data pack. The buffer sizes are determined by the Test
 * harness based on the expected shape for each component of the data pack
 * (representing either operation parameter or result of the operation performed).
 * These shapes are operation-dependent and are documented for each operation.
 * It is the responsibility of the back-end to keep track of these shapes (through
 * the opaque handle) so that they can be retrieved appropriately.
 *
 * Back-end must decode the data from \p h_plaintext handle and fill out as
 * much as it can into each data buffer. If some data buffer's size if smaller
 * than needed for that data entry, the rest of the encoded data entry must be
 * discarded.
 *
 * This function is the inverse of encode(), so, decoding the result of
 * encode(), must produce the exact same bits in \p p_native 's data buffers.
 * @sa encode()
 */
extern "C" ErrorCode decode(Handle h_benchmark,
                            Handle h_plaintext,
                            PackedData *p_native);

/**
 * @brief Encrypts a plain text into a cipher text.
 * @param[in] h_benchmark Handle to the initialized benchmark to perform.
 * @param[in] h_plaintext Handle to the plain text to encrypt.
 * @param[out] h_ciphertext Opaque handle to the encrypted data.
 * @return Error code.
 * @details The resulting cipher text handle is commonly used in a call
 * to the load() function to load the cipher text into the back-end for execution
 * of the operation to benchmark.
 *
 * Destroying or re-utilizing handle \p h_plaintext after this call completes shall
 * not affect the resulting cipher text.
 * @sa decrypt()
 */
extern "C" ErrorCode encrypt(Handle h_benchmark,
                             Handle h_plaintext,
                             Handle *h_ciphertext);

/**
 * @brief Decrypts a cipher text into corresponding plain text.
 * @param[in] h_benchmark Handle to the initialized benchmark to perform.
 * @param[in] h_ciphertext Handle to the encrypted data to decrypt.
 * @param[out] h_plaintext Opaque handle to the decrypted data.
 * @return Error code.
 * @details The resulting plain text handle represents the encoded
 * plain text. This may be passed to a call to decode() in order to obtain
 * the raw native data.
 *
 * Destroying or re-utilizing handle \p h_ciphertext after this call completes shall
 * not affect the resulting plain text.
 * @sa encrypt()
 */
extern "C" ErrorCode decrypt(Handle h_benchmark,
                             Handle h_ciphertext,
                             Handle *h_plaintext);

/**
 * @brief Loads the specified data from the local host into the remote back-end to
 * use as parameter during a call to operate().
 * @param[in] h_benchmark Handle to the initialized benchmark to perform.
 * @param[in] h_local_packed_params Collection of handles to the data of packed
 * parameters to load into back-end remote.
 * @param[in] local_count Number of handles in array \p h_local_packed_params.
 * @param[out] h_remote Handle representing the loaded data into the back-end.
 * @return Error code.
 * @details This function exists to allow back-ends to transfer the data from the
 * host into the actual device (accelerator, remote server, etc.) that will perform
 * the operations.
 *
 * Each handle in \p h_local_packed_params represents an encoded/encrypted PackedData
 * object in the local host.
 *
 * In general, plain text parameters will be packed together under a single PackedData
 * during encoding, and encrypted parameters will be packed together under another
 * single PackedData. Handles for each PackedData are generated during encode() and
 * encrypt() calls, and thus, \p h_local_packed_params collection allows for all of these
 * handles to be loaded at the same time. If not specified in the workload description,
 * the order for PackedData handles is encrypted first, followed by plain text when
 * both types of parameters, encrypted and plain text, are present; otherwise, Test
 * harness will use a single PackedData for all parameters as such:
 *
 * All parameters encrypted, or all parameters plain text:
 * @code
 * h_local_packed_params[0] <- handle to PackedData containing all parameters.
 * local_count = 1
 * @endcode
 *
 * Mix of encrypted and plain text parameters:
 * @code
 * h_local_packed_params[0] <- handle to PackedData containing encrypted parameters.
 * h_local_packed_params[1] <- handle to PackedData containing plain text parameters.
 * local_count = 2
 * @endcode
 *
 * Destroying or re-utilizing collection and/or handles in \p h_local_packed_params after
 * this call completes shall not affect the resulting \p h_remote handle.
 * @sa operate(), store()
 */
extern "C" ErrorCode load(Handle h_benchmark,
                          const Handle *h_local_packed_params,
                          std::uint64_t local_count,
                          Handle *h_remote);

/**
 * @brief Retrieves the specified data from the back-end.
 * @param[in] h_benchmark Handle to the initialized benchmark to perform.
 * @param[in] h_remote Handle to the data to retrieve from back-end remote and store
 * into \p h_local.
 * @param[out] h_local_packed_params Collection of opaque handles to represent the data
 * stored in the local host retrieved from the back-end.
 * @param[in] local_count Number of available handles to receive data.
 * @return Error code.
 * @details This function is the counterpart to load().
 *
 * Function operate() receives an opaque handle that represents the result
 * of the operation in the back-end. Because this result may live in a device
 * other than the host (accelerator, remote server, etc.), a call to `store()`
 * must be issued to retrieve the data from the back-end remote and store it in the
 * host.
 *
 * Each handle in \p h_local_packed_params will represent an encoded/encrypted PackedData
 * object after stored in the local host. If there are not enough handles in
 * \p h_local_packed_params to store all the data from \p h_remote, extra remote data
 * will be ignored. Any extra handles will be padded with zeroes.
 *
 * Destroying or re-using handle \p h_remote after this call completes shall
 * not affect the resulting \p h_local_packed_params handle.
 * @sa load(), operate()
 */
extern "C" ErrorCode store(Handle h_benchmark,
                           Handle h_remote,
                           Handle *h_local_packed_params,
                           std::uint64_t local_count);

/**
 * @brief Performs the workload operation of the benchmark.
 * @param[in] h_benchmark Handle to the initialized benchmark to perform.
 * @param[in] h_remote_packed_params Handle representing the parameters for the
 * operation previously loaded into the back-end remote by a call to load() to
 * use for the operation. The data represented by this handle must not be changed
 * since it may be reused in several calls to `operate()`.
 * @param[in] p_param_indexers Indexers for the parameters of the operation.
 * @param[out] h_remote_output Handle representing the result of the operation
 * stored in the back-end remote.
 * @return Error code.
 * @details This is the function that Test harness will benchmark. Back-end MUST ensure
 * that the complete operation runs on all specified inputs and that the corresponding
 * outputs are generated before this function returns.
 *
 * Destroying or re-using handle or values in \p h_remote_packed_params and \p p_param_indexers
 * after this call completes shall not affect the resulting \p h_remote_output.
 *
 * The actual collection of inputs to the operation that back-end must use is loaded into
 * the back-end remote using a single call of the load() function. Parameter \p p_param_indexers
 * contains the indexers for the parameter packs, and thus the number of elements in
 * array \p p_param_indexers is, at least, the same as the number of parameters required for
 * the operation. The actual configuration for the call to `operate()` issued by Test harness
 * is defined for each workload.
 *
 * Once decoded, the workload results generated by this function must be indexed starting
 * at 0, regardless of starting input sample index specified by \p p_param_indexers. Any
 * offset in the input sample index will be taken into account by Test harness when validating
 * the results. For more information, see \ref results_order .
 *
 * The following example shows a typical flow followed by Test harness when performing a benchmark.
 * Here, Test harness is benchmarking offline an operation of the form
 * @code
 * result = op(A, B)
 * @endcode
 * where parameter A has a single sample, and B has 10 samples. There is a result for each
 * pair of values. Both parameters are encrypted. The pseudo-code submits all 10 values for B.
 * Benchmarking statements are omitted for clarity.
 *
 * @code
 * constexpr int A_count = 1;
 * constexpr int B_count = 10;
 * NativeDataBuffer p_raw_inputs_A[A_count];
 * NativeDataBuffer p_raw_inputs_B[B_count];
 * // fill up the inputs
 * //...
 * //
 *
 * Handle h_engine, h_benchmark;
 * // initialize the engine and benchmark
 * //...
 * //
 *
 * // prepare the parameters for encoding
 *
 * constexpr int op_params_count = 2; // operation has 2 parameters
 *
 * DataPack param_packs[op_params_count];
 * param_packs[0].p_buffers = p_raw_inputs_A;
 * param_packs[0].buffer_count = A_count;
 * param_packs[0].param_position = 0; // A is the first parameter, so, position 0
 * param_packs[1].p_buffers = p_raw_inputs_B;
 * param_packs[1].buffer_count = B_count;
 * param_packs[1].param_position = 1; // B is the second parameter, so, position 1
 *
 * PackedData packed_parameters;
 * packed_parameters.p_data_packs = param_packs;
 * packed_parameters.pack_count = op_params_count;
 *
 * // encode the raw parameters data
 * Handle h_encoded_inputs;
 * encode(h_benchmark, &packed_parameters, &h_encoded_inputs);
 *
 * // encrypt the encoded data
 * Handle h_cipher_inputs;
 * encrypt(h_benchmark, h_encoded_inputs, &h_cipher_inputs);
 *
 * // load encrypted data into back-end's remote to use as input to the operation
 * Handle h_remote_inputs;
 * load(h_benchmark,
 *      &h_cipher_inputs, 1, // only 1 PackedData
 *      &h_remote_inputs);
 *
 * // clean up data we no longer need (in reverse order of creation)
 * destroyHandle(h_cipher_inputs);
 * destroyHandle(h_encoded_inputs);
 *
 * // prepare parameter indexers for operation
 *
 * ParameterIndexer params[op_params_count]; // one indexer per parameter
 * params[0].batch_size = 1;  // for parameter A we will be using one value
 * params[0].value_index = 0; // starting with the first value
 *
 * params[1].batch_size = 10; // for parameter B we will be using 10 values
 * params[1].value_index = 0; // starting with the first.
 *
 * // operate
 * Handle h_remote_result;
 * while (!elapsed(min_test_time))
 * {
 *     operate(h_benchmark,
 *             h_remote_inputs, params,
 *             &h_remote_result);
 * }
 *
 * // at this point, the operation must be completed in remote back-end
 *
 * // clean up data we no longer need
 * destroyHandle(h_remote_inputs);
 *
 * // postprocess output
 * Handle h_cipher_output;
 *
 * // retrieve data from back-end's remote and store in host
 * store(h_benchmark, h_remote_result,
 *       &h_cipher_output, 1 // Only 1 local PackedData for result expected for this operation.
 *      );
 *
 * // clean up data we no longer need
 * destroyHandle(h_remote_result);
 *
 * // decrypt results
 * Handle h_plain_result;
 * decrypt(h_benchmark, h_cipher_output, &h_plain_result);
 *
 * // clean up data we no longer need
 * destroyHandle(h_cipher_output);
 *
 * // allocate space for decoded results
 *
 * NativeDataBuffer p_raw_results[A_count * B_count]; // allocate space for all 10 outputs
 * // allocate memory for raw outputs
 * // ...
 * //
 *
 * DataPack results_pack;
 * results_pack.p_buffers = p_raw_results;
 * results_pack.buffer_count = A_count * B_count;
 * results_pack.param_position = 0; // we are retrieving results in the first position into this data pack
 *
 * PackedData packed_results;
 * packed_results.p_data_packs = &results_pack;
 * packed_results.pack_count = 1; // result shape is [1, 10]
 *
 * // allocate memory for raw outputs
 * // ...
 * //
 *
 * // decode the results
 * decode(Handle h_benchmark, h_plain_result, &packed_results);
 *
 * // clean up data we no longer need
 * destroyHandle(h_plain_result);
 *
 * // benchmark completed
 * destroyHandle(h_benchmark);
 *
 * // validate output
 * // ...
 * //
 *
 * // clean up packed_results
 * // ...
 * //
 *
 * // when done with all the benchmarks, clean up back-end
 * destroyHandle(h_engine);
 * @endcode
 * @sa load(), store(), \ref results_order
 */
extern "C" ErrorCode operate(Handle h_benchmark,
                             Handle h_remote_packed_params,
                             const ParameterIndexer *p_param_indexers,
                             Handle *h_remote_output);

/**
 * @brief Retrieves the name of a specified scheme ID from the back-end.
 * @param[in] h_engine Handle to the back-end engine.
 * @param[in] s Scheme ID.
 * @param[out] p_name Buffer to store C-string name of the scheme.
 * @param[in] size Size in bytes of \p p_name buffer.
 * @return Number of bytes needed in buffer to store the complete name,
 * including the C-string null terminator, or zero if error.
 * @details If \p p_name is null, this function performs no copy. Use
 * this behavior to obtain the size, in bytes, required to store the complete
 * name of the scheme, including the C-string null terminator.
 *
 * If \p p_name is not null, this function copies as many bytes as specified
 * in \p size from the scheme's full name into the buffer pointed to by
 * \p p_name, including the C-string null terminator.
 *
 * This function is used for report purposes.
 */
extern "C" std::uint64_t getSchemeName(Handle h_engine, Scheme s, char *p_name, std::uint64_t size);
/**
 * @brief Retrieves the name of the specified security for the scheme ID from the back-end.
 * @param[in] h_engine Handle to the back-end engine.
 * @param[in] s Scheme ID.
 * @param[in] sec Security ID for scheme.
 * @param[out] p_name Buffer to store C-string name of the security.
 * @param[in] size Size in bytes of \p p_name buffer.
 * @return Number of bytes needed in buffer to store the complete name,
 * including the C-string null terminator, or zero if error.
 * @details If \p p_name is null, this function performs no copy. Use
 * this behavior to obtain the size, in bytes, required to store the complete
 * name of the security scheme, including the C-string null terminator.
 *
 * If \p p_name is not null, this function copies as many bytes as specified
 * in \p size from the security scheme's full name into the buffer pointed to by
 * \p p_name, including the C-string null terminator.
 *
 * This function is used for report purposes.
 */
extern "C" std::uint64_t getSchemeSecurityName(Handle h_engine, Scheme s, Security sec,
                                               char *p_name, std::uint64_t size);

/**
 * @brief Retrieves back-end specific text description for a benchmark descriptor.
 * @param[in] h_engine Handle to the back-end engine.
 * @param[in] h_bench_desc Handle to benchmark description for which to obtain the
 * text description.
 * @param[in] p_w_params Workload parameters for this benchmark. May be null if no
 * workload parameters are needed or their description is not required.
 * @param[out] p_description Buffer to store C-string description of the benchmark.
 * @param[in] size Size in bytes of \p p_description buffer.
 * @return Number of bytes needed in buffer to store the complete description,
 * including the C-string null terminator, or zero if error.
 * @details If \p p_description is null, this function performs no copy. Use
 * this behavior to obtain the size, in bytes, required to store the complete
 * description, including the C-string null terminator.
 *
 * If \p p_description is not null, this function copies as many bytes as specified
 * in \p size from the benchmark text description into the buffer pointed to by
 * \p p_description, including the C-string null terminator.
 *
 * This description is used along with other text descriptions about the workload,
 * data types, encryption schemes, etc, to produce the benchmark report. It is
 * intended to supply any extra description that is specific to the general-purpose
 * fields in `BenchmarkDescriptor` structure (such as `other`) that is not specified
 * by getSchemeName(), getSchemeSecurityName() and other similar functions.
 */
extern "C" std::uint64_t getBenchmarkDescriptionEx(Handle h_engine,
                                                   Handle h_bench_desc,
                                                   const hebench::APIBridge::WorkloadParams *p_w_params,
                                                   char *p_description, std::uint64_t size);

/**
 * @brief Retrieves the general error description of an error code.
 * @param[in] code Error code to describe.
 * @param[out] p_description Buffer to store C-string description of the error.
 * @param[in] size Size in bytes of \p p_description buffer.
 * @return Number of bytes needed in buffer to store the complete description,
 * including the C-string null terminator, or zero if error.
 * @details If \p p_description is null, this function performs no copy. Use
 * this behavior to obtain the size, in bytes, required to store the complete
 * description of the security scheme, including the C-string null terminator.
 *
 * If \p p_description is not null, this function copies as many bytes as specified
 * in \p size from the error description into the buffer pointed to by
 * \p p_description, including the C-string null terminator.
 */
extern "C" std::uint64_t getErrorDescription(ErrorCode code, char *p_description, std::uint64_t size);
/**
 * @brief Retrieves the detailed description of the last error that occurred
 * during an operation on the engine.
 * @param[in] h_engine Handle to the back-end engine.
 * @param[out] p_description Buffer to store C-string description of the error.
 * @param[in] size Size in bytes of \p p_description buffer.
 * @return Number of bytes needed in buffer to store the complete description,
 * including the C-string null terminator, or zero if error.
 * @details If \p p_description is null, this function performs no copy. Use
 * this behavior to obtain the size, in bytes, required to store the complete
 * description of the security scheme, including the C-string null terminator.
 *
 * If \p p_description is not null, this function copies as many bytes as specified
 * in \p size from the error description into the buffer pointed to by
 * \p p_description, including the C-string null terminator.
 *
 * Detailed description is back-end implementation defined. If no detailed
 * implementation, returning the general error description is suggested.
 * If no error has occurred, some "no error" message is suggested.
 */
extern "C" std::uint64_t getLastErrorDescription(Handle h_engine, char *p_description, std::uint64_t size);

} // namespace APIBridge
} // namespace hebench

#endif // defined _HEBench_API_Bridge_H_7e5fa8c2415240ea93eff148ed73539b
