
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_EngineObject_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_EngineObject_H_7e5fa8c2415240ea93eff148ed73539b

#include <cstdint>
#include <memory>

#include "error_handling.hpp"

namespace hebench {
namespace cpp {

class BaseEngine;

/**
 * @brief Represents an object with a tag.
 * @details The design philosophy is that objects are tagged using a bit mask.
 *
 * A backend implementation will test the tag of an opaque handle received from
 * the Test Harness against the tag mask of the expected class to see if it is of
 * the correct type and to decide the correct operations. Thus, it is recommended
 * that derived classes form their tags by combining their bit mask with the base
 * class' tag using binary OR.
 *
 * It is the client's responsibility to ensure that their tags are compatible and
 * masks are correct. This class is only offered as a helper wrapper for object tagging.
 *
 * <b>Convention</b>:
 *
 * C++ wrapper will use the most significant 8 bits of the 64-bit tag. The 56 least
 * significant bits can be used by clients as they see fit. Not respecting this
 * convention will result in undefined behavior by the C++ wrapper.
 */
class ITaggedObject
{
public:
    ITaggedObject() {}
    virtual ~ITaggedObject() {}
    /**
     * @brief Retrieves the tag of the class to which this object belongs.
     * @details Most significant 8 bits of the 64-bit tag are reserved by C++ wrapper.
     * See `ITaggedObject` for more details.
     */
    virtual std::int64_t classTag() const = 0;

    static constexpr std::int64_t MaskByteSet = 0xFF;
    /**
     * @brief Mask representing all bits reserved by C++ wrapper.
     * @details Most significant 8 bits of the 64-bit tag are reserved by C++ wrapper.
     * See `ITaggedObject` for more details.
     *
     * Any client tag value must satisfy
     * @code
     * new_tag & MaskReservedBits == 0
     * @endcode
     * to respect the C++ wrapper tag convention.
     */
    static constexpr std::int64_t MaskReservedBits = MaskByteSet << 56;
};

/**
 * @brief Helper class to encapsulate objects that will cross the boundary
 * of the API Bridge.
 * @details This class is provided to ease the creation of handles to backend
 * internal objects (other than engine and benchmarks).
 *
 * It is recommended to use BaseEngine::createHandle<ObjType>() to create
 * HEBench opaque handles that wrap objects in an instance of this class
 * that can cross the boundary of the API bridge. Use method
 * BaseEngine::retrieveFromHandle<ObjType>() to retrieve the wrapped object,
 * and BaseEngine::duplicateHandle() to create reference-counted handle duplicates.
 *
 * When using this object directly, instead of the recommended wrappers, users
 * should keep in mind that the method `T &EngineObject::get<T>()` allows the
 * retrieval of a reference to the created object of type `T` by the backend
 * when needed. However, the casting of the internal object by this method is
 * <b>unsafe</b>. Users must ensure that the object contained is of the right
 * type (such as using the `tag` field in the handle for extra information).
 *
 * Implementation of the API Bridge by this C++ wrapper will properly free
 * and destroy pointers to EngineObject instances, and calling the appropriate
 * destructor of the wrapped object of type `T` when reference counts reach 0,
 * if assigned to handles as follows:
 *
 * @code
 * EngineObject *p_engine_object = ...
 * hebench::APIBridge::Handle handle;
 * handle.p    = p_engine_object;
 * handle.size = ... // backend implementation dependent
 * handle.tag  = p_engine_object->classTag() | other_bit_mask_tag_values;
 * @endcode
 *
 * For the automation to work well, the handle tag must satify:
 *
 * @code
 * handle.tag & EngineObject::tag == EngineObject::tag
 * @endcode
 */
class EngineObject final : public ITaggedObject
{
private:
    HEBERROR_DECLARE_CLASS_NAME(EngineObject)

public:
    /**
     * @brief Used to identify this class when returned as a handle to Test Harness.
     * @sa ITaggedObject
     */
    static constexpr std::int64_t tag = 0x2000000000000000; // bit 61

    EngineObject(const BaseEngine &engine, std::shared_ptr<void> p_obj) :
        m_engine(engine), m_p_obj(p_obj)
    {
        if (!p_obj)
            throw std::invalid_argument(HEBERROR_MSG_CLASS("Invalid null pointer: p_obj"));
    }
    EngineObject(const EngineObject &src) :
        EngineObject(src.engine(), src.m_p_obj)
    {
    }
    ~EngineObject() override {}
    EngineObject &operator=(const EngineObject &src)
    {
        if (this != &src)
        {
            if (&m_engine != &src.m_engine)
                throw std::runtime_error(HEBERROR_MSG_CLASS("Engine mismatch."));
            if (!src.m_p_obj)
                throw std::invalid_argument(HEBERROR_MSG_CLASS("Invalid null pointer: src.m_p_obj"));
            this->m_p_obj = src.m_p_obj;
        } // end if
        return *this;
    }

    const BaseEngine &engine() const { return m_engine; }

    template <class T>
    T &get()
    {
        return *reinterpret_cast<T *>(m_p_obj.get());
    }
    template <class T>
    const T &get() const
    {
        return *reinterpret_cast<T *>(m_p_obj.get());
    }

    std::int64_t classTag() const override { return EngineObject::tag; }

private:
    const BaseEngine &m_engine;
    std::shared_ptr<void> m_p_obj;
};

} // namespace cpp
} // namespace hebench

#endif // defined _HEBench_API_Bridge_EngineObject_H_7e5fa8c2415240ea93eff148ed73539b
