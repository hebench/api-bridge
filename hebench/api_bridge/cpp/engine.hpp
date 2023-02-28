
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_Base_Engine_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_Base_Engine_H_7e5fa8c2415240ea93eff148ed73539b

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "engine_object.hpp"
#include "hebench/api_bridge/types.h"

namespace hebench {
namespace cpp {

class BaseEngine;
class BenchmarkDescription;
class BaseBenchmark;

/**
 * @brief Bundles a benchmark and its description to cross the API Bridge as a handle.
 */
struct BenchmarkHandle
{
    BaseBenchmark *p_benchmark;
    BenchmarkDescription *p_bench_description;
};

/**
 * @brief Creates a new BaseEngine object.
 * @param[in] p_buffer Input buffer of bytes with extra information for engine
 * initialization. May be `null` if no initialization data is available.
 * @param[in] size Number of bytes pointed by \p p_buffer .
 * @return A pointer to the newly created object.
 * @details
 * This is only a function prototype. Backends extending this C++ wrapper should provide
 * an implementation to this function.
 *
 * This function is used by the C++ wrapper to create the engine during initialization, and
 * it is supposed to return a polymorphic pointer to the concrete engine class that will
 * drive the backend.
 *
 * Data in \p p_buffer is specified by frontend user through benchmark
 * configuration files. Test Harness will forward this information to backend
 * using this buffer. Utilization of this information is backend specific, and backends
 * can ignore it.
 */
BaseEngine *createEngine(const std::int8_t *p_buffer, std::uint64_t size); // implement this
/**
 * @brief Destroys and cleans up resources from an engine created by createEngine().
 * @param[in] p Pointer to engine to destroy.
 * @details
 * This is only a function prototype. Backends extending this C++ wrapper should provide
 * an implementation to this function.
 *
 * This function is used by the C++ wrapper to destroy the engine during clean up.
 */
void destroyEngine(BaseEngine *p); // implement this

/**
 * @brief Base class that encapsulates common behavior of backend engines.
 * @details Any backend engine must derive from this class in order to take
 * advantage of the C++ wrapper boilerplate automation.
 */
class BaseEngine : public ITaggedObject
{
private:
    HEBERROR_DECLARE_CLASS_NAME(BaseEngine)

public:
    BaseEngine(const BaseEngine &) = delete;
    BaseEngine &operator=(const BaseEngine &) = delete;

    BaseEngine(BaseEngine &&) = delete;
    BaseEngine &operator=(BaseEngine &&) = delete;

public:
    /**
     * @brief Used to identify this class when returned as a handle to Test Harness.
     * @sa ITaggedObject
     */
    static constexpr std::int64_t tag = 0x8000000000000000; // bit 63
public:
    ~BaseEngine() override {}

    std::int64_t classTag() const override { return tag; }

    /**
     * @brief Retrieves the description of a specific error code.
     * @return A string containing the error description, or a default message if
     * error code is not known.
     * @details The error code must a standard error code defined in the API bridge
     * or a custom code added through a call to addErrorCode() during this object's
     * initialization.
     * @sa
     */
    static const std::string &getErrorDesc(hebench::APIBridge::ErrorCode err_code);
    /**
     * @brief Retrieves the last error that occurred as set by setLastError().
     */
    static hebench::APIBridge::ErrorCode getLastError() { return m_last_error; }
    /**
     * @brief Retrieves the description of the last error that occurred as
     * set by setLastError().
     */
    static const std::string &getLastErrorDesc() { return m_s_last_error_description; }
    /**
     * @brief Sets the last error code that occurred.
     * @param[in] value Error code.
     * @details This method is automatically called by C++ wrapper when an
     * exception of type HEBenchError is caught.
     */
    static void setLastError(hebench::APIBridge::ErrorCode value);
    /**
     * @brief Sets the last error code that occurred.
     * @param[in] value Error code.
     * @param[in] err_desc Description of the error.
     * @details This method is automatically called by C++ wrapper when an
     * exception of type HEBenchError is caught.
     */
    static void setLastError(hebench::APIBridge::ErrorCode value,
                             const std::string &err_desc);

    /**
     * @brief Retrieves backend specific text description for a benchmark descriptor.
     * @sa hebench::APIBridge::getBenchmarkDescriptionEx()
     */
    std::string getBenchmarkDescriptionEx(hebench::APIBridge::Handle h_bench_desc,
                                          const hebench::APIBridge::WorkloadParams *p_w_params) const;

    /**
     * @brief Retrieves a dictionary that maps schemes known to this benchmark
     * to their human readable names.
     * @sa hebench::APIBridge::getSchemeName()
     */
    const std::unordered_map<hebench::APIBridge::Scheme, std::string> &
    schemeName() const { return m_map_scheme_name; }
    /**
     * @brief Retrieves a dictionary that maps security types known to this benchmark
     * to their human readable names.
     * @sa hebench::APIBridge::getSchemeSecurityName()
     */
    const std::unordered_map<hebench::APIBridge::Security, std::string> &
    securityName() const { return m_map_security_name; }

    /**
     * @brief Retrieves the number of benchmarks for which the backend is registering
     * to perform.
     * @sa hebench::APIBridge::subscribeBenchmarksCount()
     */
    std::uint64_t subscribeBenchmarkCount() const { return m_descriptors.size(); }
    /**
     * @brief Retrieves the benchmarks for which the backend is registering to perform.
     * @sa hebench::APIBridge::subscribeBenchmarks()
     */
    void subscribeBenchmarks(hebench::APIBridge::Handle *p_h_bench_descs, std::uint64_t count) const;
    /**
     * @brief Retrieves the number of workload parameters for the decribed benchmark.
     * @param[in] h_bench_desc Handle to benchmark description to query for information.
     */
    std::uint64_t getWorkloadParamsCount(hebench::APIBridge::Handle h_bench_desc) const;
    /**
     * @brief Retrieves the number of sets of default arguments supported by this benchmark's
     * workload.
     * @param[in] h_bench_desc Handle to benchmark description to query for information.
     */
    std::uint64_t getDefaultWorkloadParamsCount(hebench::APIBridge::Handle h_bench_desc) const;
    void describeBenchmark(hebench::APIBridge::Handle h_bench_desc,
                           hebench::APIBridge::BenchmarkDescriptor *p_bench_desc,
                           hebench::APIBridge::WorkloadParams *p_default_params,
                           std::uint64_t default_count) const;
    /**
     * @brief Creates the benchmark specified by the descriptor.
     * @sa hebench::APIBridge::initBenchmark()
     */
    hebench::APIBridge::Handle createBenchmark(hebench::APIBridge::Handle h_bench_desc,
                                               const hebench::APIBridge::WorkloadParams *p_params);
    /**
     * @brief Destroys and cleans up a benchmark created by createBenchmark().
     * @param[in] h_bench Handle to benchmark to destroy.
     * @throws HEBenchError on invalid handle with error code HEBENCH_ECODE_CRITICAL_ERROR.
     */
    void destroyBenchmark(hebench::APIBridge::Handle h_bench);

    template <class T, typename... Args>
    /**
     * @brief Encapsulates an object of type T in an opaque HEBench handle.
     * @param[in] size Value to use as the `size` filed for the handle.
     * @param[in] extra_tags Value to use as the `tag` filed for the handle.
     * This value will be combined with the value for `EngineObject::tag` using
     * binary `OR` operation.
     * @param[in] args Constructor arguments to construct the object of type T.
     * @return A handle that can cross the boundary of the API bridge.
     * @throws hebench::cpp::HEBenchError if any of the most significant 8 bits
     * of `extra_tags` is set, due to these being reserved by `ITaggedObject`.
     * @details This method will construct the object using the arguments specified
     * in \p args.
     * @sa retrieveFromHandle(), EngineObject::tag
     */
    hebench::APIBridge::Handle createHandle(std::uint64_t size, std::int64_t extra_tags,
                                            Args &&... args) const;
    /**
     * @brief Duplicates a handle created by `createhandle()`.
     * @param[in] h Handle to duplicate.
     * @param[in] new_tag New tag to set in the duplicated handle.
     * @param[in] check_tags Tags to check for. If the input handle does not match this mask
     * in a binary AND test, an exception is thrown.
     * @return A new handle that points to the same internal representation as the input handle.
     * size is also copied. This duplicate is a shallow copy. See remarks.
     * @throws hebench::cpp::HEBenchError if tag check fails, or arguments are invalid (invalid
     * `check_tags` or handle is null).
     * @details
     * Handles created by `createHandle()` point to internal representations that are reference
     * counted. This method, returns a new handle that points to the same internal representation,
     * but increases the reference count. Thus, both handles point to the same internal representation
     * (shallow copy), but destroying each only decrements the reference count. The internal
     * representation is actually destroyed when this reference count reaches 0.
     *
     * Warning: destroyed handles must not be duplicated. Attempting to duplicate a handle that
     * has been destroyed will cause undefined behavior.
     */
    hebench::APIBridge::Handle duplicateHandle(hebench::APIBridge::Handle h, std::int64_t new_tag, std::int64_t check_tags) const;
    /**
     * @brief Duplicates a handle created by `createhandle()`.
     * @param[in] h Handle to duplicate.
     * @param[in] check_tags Tags to check for. If the input handle does not match this mask
     * in a binary AND test, an exception is thrown.
     * @return A new handle that points to the same internal representation as the input handle.
     * Tags and size are also copied. This duplicate is a shallow copy. See remarks.
     * @throws hebench::cpp::HEBenchError if tag check fails, or arguments are invalid (invalid
     * `check_tags` or handle is null).
     * @details
     * Handles created by `createHandle()` point to internal representations that are reference
     * counted. This method, returns a new handle that points to the same internal representation,
     * but increases the reference count. Thus, both handles point to the same internal representation
     * (shallow copy), but destroying each only decrements the reference count. The internal
     * representation is actually destroyed when this reference count reaches 0.
     *
     * Warning: destroyed handles must not be duplicated. Attempting to duplicate a handle that
     * has been destroyed will cause undefined behavior.
     */
    hebench::APIBridge::Handle duplicateHandle(hebench::APIBridge::Handle h, std::int64_t check_tags = 0) const;
    template <class T, typename... Args>
    /**
     * @brief Retrieves the object of type T encapsulated in an opaque HEBench handle
     * by method createHandle().
     * @param[in] h Handle containing the object.
     * @param[in] extra_tags Value to check against the `tag` for the handle.
     * @return A reference to the object of type T encapsulated in the handle.
     * @throws hebench::cpp::HEBenchError if any of the most significant 8 bits
     * of `extra_tags` is set, or
     * @code
     * handle.tag & EngineObject::tag != EngineObject::tag
     * || handle.tag & extra_tags != extra_tags
     * @endcode
     * @details Note that once extracted, the object will be statically casted to type T.
     * Due to the nature of the opaque handles, this cast is <b>unsafe</b>, and thus,
     * it is user's responsibility to make sure that the encapsulated object is of the
     * right type (such as using the `tag` field in the handle for extra information).
     * @sa createHandle(), EngineObject::tag
     */
    T &retrieveFromHandle(hebench::APIBridge::Handle h, std::int64_t extra_tags = 0) const;

    template <class T, typename... Args>
    /**
     * @brief Creates an `EngineObject` that wraps an object of type `T`.
     * @param args Arguments for constructor of object of type `T`.
     * @return A pointer to an `EngineObject` instance that wraps the newly constructed
     * object of type `T`.
     * @details Pointers to `EngineObject` instances make easier to wrap
     * backend internal objects that need to cross the boundary of the API bridge. For
     * easier use, see method createHandle().
     * @sa createHandle(), retrieveFromHandle()
     */
    EngineObject *createEngineObj(Args &&... args) const;
    template <class T, typename... Args>
    /**
     * @brief Creates a smart pointer of the specified template type.
     * @param args Arguments for constructor of object of specified type to be created.
     * @return std::shared_ptr of the specified template type.
     * @details The RAII and reference counting properties of smart pointer ensure
     * that the created object is properly destroyed when it goes out of scope and
     * it is the last reference.
     *
     * The allocation and deallocation mechanisms of the smart pointer use
     * BaseEngine::createObj() and BaseEngine::destroyObj() methods in order
     * to keep the BaseEngine as the allocation manager.
     */
    std::shared_ptr<T> createRAII(Args &&... args) const;
    template <class T, typename... Args>
    T *createObj(Args &&... args) const;
    template <class T>
    void destroyObj(T *p) const
    {
        if (p)
            delete p;
    }

protected:
    BaseEngine();
    /**
     * @brief Initializes the backend engine and populates the backend description.
     * @details
     * This method is called automatically during engine initialization.
     *
     * Derived classes should use this method to initialize this class and populate
     * the descriptions for the benchmarks that this backend will perform; and add
     * error descriptions, scheme and security names for this backend.
     *
     * @sa addBenchmarkDescription(), addErrorCode(), addSchemeName(), addSecurityName()
     */
    virtual void init() = 0;

    /**
     * @brief Retrieves the `BenchmarkDescription` object pointed by the specified handle.
     * @param[in] h_desc Handle to descriptor as returned by subscribeBenchmarks().
     * @return A smart pointer to the `BenchmarkDescription` object that is pointed by the
     * specified handle or a null pointer if no match was found.
     * @sa subscribeBenchmarks()
     */
    std::shared_ptr<BenchmarkDescription> matchBenchmark(hebench::APIBridge::Handle h_desc) const;

    /**
     * @brief Adds a new benchmark to the list of benchmarks to register by this
     * backend.
     * @param[in] p_desc Object describing the new benchmark.
     * @details This method is to be called during engine initialization.
     * @sa init()
     */
    void addBenchmarkDescription(std::shared_ptr<BenchmarkDescription> p_desc);
    /**
     * @brief Adds a new supported scheme and name pair to the list of schemes
     * supported by this backend.
     * @param[in] scheme Scheme ID.
     * @param[in] name Corresponding name for the scheme.
     * @details This method is to be called during engine initialization.
     * @sa init(), hebench::APIBridge::getSchemeName()
     */
    void addSchemeName(hebench::APIBridge::Scheme scheme, const std::string &name);
    /**
     * @brief Adds a new supported security and name pair to the list of securitty types
     * supported by this backend.
     * @param[in] security Security ID.
     * @param[in] name Corresponding name for the new security type.
     * @details This method is to be called during engine initialization.
     * @sa init(), hebench::APIBridge::getSchemeSecurityName()
     */
    void addSecurityName(hebench::APIBridge::Security security, const std::string &name);
    /**
     * @brief Adds a new error code and the error description to the list of possible
     * error codes that this backend can generate.
     * @param[in] code Error code.
     * @param[in] description Description of the error as it will returned by
     * hebench::APIBridge::getErrorDescription().
     * @details This method is to be called during engine initialization.
     * @sa init(), hebench::APIBridge::getErrorDescription()
     */
    static void addErrorCode(hebench::APIBridge::ErrorCode code, const std::string &description);

private:
    static const std::string UnknownErrorMsg;
    static hebench::APIBridge::ErrorCode m_last_error;
    static std::string m_s_last_error_description;
    static std::unordered_map<hebench::APIBridge::ErrorCode, std::string> m_map_error_desc;

    std::vector<std::shared_ptr<BenchmarkDescription>> m_descriptors;
    std::unordered_map<hebench::APIBridge::Scheme, std::string> m_map_scheme_name;
    std::unordered_map<hebench::APIBridge::Security, std::string> m_map_security_name;
};

template <class T, typename... Args>
hebench::APIBridge::Handle BaseEngine::createHandle(std::uint64_t size, std::int64_t extra_tags,
                                                    Args &&... args) const
{
    if ((extra_tags & ITaggedObject::MaskReservedBits) != 0)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid 'extra_tags' detected. Most significant 8 bits of tags are reserved."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);

    hebench::cpp::EngineObject *p_retval = createEngineObj<T>(std::forward<Args>(args)...);

    hebench::APIBridge::Handle retval;
    retval.p    = p_retval;
    retval.size = size;
    retval.tag  = p_retval->classTag() | extra_tags;

    return retval;
}

template <class T, typename... Args>
T &BaseEngine::retrieveFromHandle(hebench::APIBridge::Handle h, std::int64_t extra_tags) const
{
    if ((extra_tags & ITaggedObject::MaskReservedBits) != 0)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid 'extra_tags' detected. Most significant 8 bits of tags are reserved."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    if ((h.tag & hebench::cpp::EngineObject::tag) != hebench::cpp::EngineObject::tag)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid tag detected. Expected EngineObject::tag."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    if ((h.tag & extra_tags) != extra_tags)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid tag detected. Expected " + std::to_string(extra_tags) + "."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);

    if (!h.p)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid null handle."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);

    // retrieve our internal format object from the handle
    hebench::cpp::EngineObject *p_obj = reinterpret_cast<hebench::cpp::EngineObject *>(h.p);
    return p_obj->get<T>();
}

template <class T, typename... Args>
EngineObject *BaseEngine::createEngineObj(Args &&... args) const
{
    std::shared_ptr<T> raii = createRAII<T>(std::forward<Args>(args)...);
    EngineObject *retval    = new EngineObject(*this, raii);
    return retval;
}

template <class T, typename... Args>
std::shared_ptr<T> BaseEngine::createRAII(Args &&... args) const
{
    std::shared_ptr<T> retval = std::shared_ptr<T>(this->createObj<T>(std::forward<Args>(args)...),
                                                   [this](T *p) {
                                                       this->destroyObj<T>(p);
                                                   });
    return retval;
}

template <class T, typename... Args>
T *BaseEngine::createObj(Args &&... args) const
{
    return new T(std::forward<Args>(args)...);
}

} // namespace cpp
} // namespace hebench

#endif // defined _HEBench_API_Bridge_Base_Engine_H_7e5fa8c2415240ea93eff148ed73539b
