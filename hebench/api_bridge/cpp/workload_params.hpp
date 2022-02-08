
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_WorkloadParamsCPP_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_WorkloadParamsCPP_H_7e5fa8c2415240ea93eff148ed73539b

#include <stdexcept>
#include <string>
#include <vector>

#include "hebench/api_bridge/types.h"
#include "utilities.hpp"

namespace hebench {
namespace cpp {
namespace WorkloadParams {

/**
 * @brief Base wrapper around the flexible workload parameters.
 * @details This class helps to hide nuisances with workload parameters
 * values and types using templated methods. The templates are defined
 * only for the types supported by the flexible workload parameters.
 *
 * This class also serves as base class for wrappers for specific workloads.
 * Clients can use this class as is, or use one of the specializations.
 * Note that some specializations may handle nuisances of the workload
 * parameters definition for the workload they represent, so, it is
 * recommended to use the specializations when possible.
 */
class Common
{
public:
    Common(std::size_t num_params = 0) :
        m_w_params(num_params)
    {
    }
    Common(const std::vector<hebench::APIBridge::WorkloadParam> &w_params) :
        m_w_params(w_params)
    {
    }
    Common(const hebench::APIBridge::WorkloadParams &w_params) :
        m_w_params(w_params.params, w_params.params + w_params.count)
    {
    }
    virtual ~Common() = default;

    template <typename T>
    /**
     * @brief Adds a new workload parameter to the list.
     * @param[in] value Value for the parameter.
     * @param[in] name Name for the parameter.
     */
    void add(const T &value, const std::string &name = std::string());
    template <typename T>
    /**
     * @brief Sets a new value for an existing workload parameter.
     * @param[in] index Index for the parameter to set.
     * @param[in] value New value for the parameter.
     * @param[in] name New name for the parameter.
     * @throws std::out_of_range if \p index is out of range.
     */
    void set(std::size_t index, const T &value, const std::string &name);
    template <typename T>
    /**
     * @brief Sets a new value for an existing workload parameter.
     * @param[in] index Index for the parameter to set.
     * @param[in] value New value for the parameter.
     * @throws std::out_of_range if \p index is out of range.
     */
    void set(std::size_t index, const T &value);
    template <typename T>
    /**
     * @brief Retrieves the value for an existing workload parameter.
     * @param[in] index Index for the parameter to retrieve.
     * @returns The value contained in the specified workload parameter
     * of the specified type.
     * @throws std::out_of_range if \p index is out of range.
     */
    const T &get(std::size_t index) const;
    const char *getName(std::size_t index) const { return getParams().at(index).name; }

    const std::vector<hebench::APIBridge::WorkloadParam> &getParams() const { return m_w_params; }

protected:
    std::vector<hebench::APIBridge::WorkloadParam> m_w_params;
};

//-------------------------------------------------------
// class WorkloadParams::Generic template specialization
//-------------------------------------------------------

// Int64

template <>
inline void Common::add<std::int64_t>(const std::int64_t &value, const std::string &name)
{
    m_w_params.push_back(hebench::APIBridge::WorkloadParam());
    hebench::APIBridge::WorkloadParam &w_param = m_w_params.back();

    w_param.data_type = hebench::APIBridge::WorkloadParamType::Int64;
    w_param.i_param   = value;
    Utilities::copyString(w_param.name, HEBENCH_MAX_BUFFER_SIZE, name);
}

template <>
inline void Common::set<std::int64_t>(std::size_t index, const std::int64_t &value, const std::string &name)
{
    hebench::APIBridge::WorkloadParam &w_param = m_w_params.at(index);

    w_param.data_type = hebench::APIBridge::WorkloadParamType::Int64;
    w_param.i_param   = value;
    Utilities::copyString(w_param.name, HEBENCH_MAX_BUFFER_SIZE, name);
}

template <>
inline void Common::set<std::int64_t>(std::size_t index, const std::int64_t &value)
{
    hebench::APIBridge::WorkloadParam &w_param = m_w_params.at(index);

    w_param.data_type = hebench::APIBridge::WorkloadParamType::Int64;
    w_param.i_param   = value;
}

template <>
inline const std::int64_t &Common::get<std::int64_t>(std::size_t index) const
{
    return m_w_params.at(index).i_param;
}

// UInt64

template <>
inline void Common::add<std::uint64_t>(const std::uint64_t &value, const std::string &name)
{
    m_w_params.push_back(hebench::APIBridge::WorkloadParam());
    hebench::APIBridge::WorkloadParam &w_param = m_w_params.back();

    w_param.data_type = hebench::APIBridge::WorkloadParamType::UInt64;
    w_param.u_param   = value;
    Utilities::copyString(w_param.name, HEBENCH_MAX_BUFFER_SIZE, name);
}

template <>
inline void Common::set<std::uint64_t>(std::size_t index, const std::uint64_t &value, const std::string &name)
{
    hebench::APIBridge::WorkloadParam &w_param = m_w_params.at(index);

    w_param.data_type = hebench::APIBridge::WorkloadParamType::UInt64;
    w_param.u_param   = value;
    Utilities::copyString(w_param.name, HEBENCH_MAX_BUFFER_SIZE, name);
}

template <>
inline void Common::set<std::uint64_t>(std::size_t index, const std::uint64_t &value)
{
    hebench::APIBridge::WorkloadParam &w_param = m_w_params.at(index);

    w_param.data_type = hebench::APIBridge::WorkloadParamType::UInt64;
    w_param.u_param   = value;
}

template <>
inline const std::uint64_t &Common::get<std::uint64_t>(std::size_t index) const
{
    return m_w_params.at(index).u_param;
}

// Float64

template <>
inline void Common::add<double>(const double &value, const std::string &name)
{
    m_w_params.push_back(hebench::APIBridge::WorkloadParam());
    hebench::APIBridge::WorkloadParam &w_param = m_w_params.back();

    w_param.data_type = hebench::APIBridge::WorkloadParamType::Float64;
    w_param.f_param   = value;
    Utilities::copyString(w_param.name, HEBENCH_MAX_BUFFER_SIZE, name);
}

template <>
inline void Common::set<double>(std::size_t index, const double &value, const std::string &name)
{
    hebench::APIBridge::WorkloadParam &w_param = m_w_params.at(index);

    w_param.data_type = hebench::APIBridge::WorkloadParamType::Float64;
    w_param.f_param   = value;
    Utilities::copyString(w_param.name, HEBENCH_MAX_BUFFER_SIZE, name);
}

template <>
inline void Common::set<double>(std::size_t index, const double &value)
{
    hebench::APIBridge::WorkloadParam &w_param = m_w_params.at(index);

    w_param.data_type = hebench::APIBridge::WorkloadParamType::Float64;
    w_param.f_param   = value;
}

template <>
inline const double &Common::get<double>(std::size_t index) const
{
    return m_w_params.at(index).f_param;
}

//-------------------------------------------
// Workload parameter specialization classes
//-------------------------------------------

class MatrixMultiply : public Common
{
public:
    enum : std::size_t
    {
        Index_RowsM0,
        Index_ColsM0,
        Index_ColsM1,
        MinRequiredParameters
    };

    /**
     * @brief Number of rows in matrix M0.
     */
    std::uint64_t rows_M0() const { return get<std::uint64_t>(Index_RowsM0); }
    /**
     * @brief Number of rows in matrix M0.
     */
    std::uint64_t &rows_M0() { return m_w_params.at(Index_RowsM0).u_param; }
    /**
     * @brief Number of columns in matrix M0 and rows in matrix M1.
     */
    std::uint64_t cols_M0() const { return get<std::uint64_t>(Index_ColsM0); }
    /**
     * @brief Number of columns in matrix M0 and rows in matrix M1.
     */
    std::uint64_t &cols_M0() { return m_w_params.at(Index_ColsM0).u_param; }
    /**
     * @brief Number of columns in matrix M1.
     */
    std::uint64_t cols_M1() const { return get<std::uint64_t>(Index_ColsM1); }
    /**
     * @brief Number of columns in matrix M1.
     */
    std::uint64_t &cols_M1() { return m_w_params.at(Index_ColsM1).u_param; }

public:
    /**
     * @brief Initializes a new object to represent workload parameters for matrix
     * multiplication workload.
     * @param[in] _rows_M0 Number of rows in matrix M0.
     * @param[in] _cols_M0 Number of columns in matrix M0 and rows in matrix M1.
     * @param[in] _cols_M1 Number of columns in matrix M1.
     */
    MatrixMultiply(std::uint64_t _rows_M0 = 0, std::uint64_t _cols_M0 = 0, std::uint64_t _cols_M1 = 0) :
        Common(MinRequiredParameters)
    {
        this->set<std::uint64_t>(Index_RowsM0, _rows_M0, "rows_M0");
        this->set<std::uint64_t>(Index_ColsM0, _cols_M0, "cols_M0");
        this->set<std::uint64_t>(Index_ColsM1, _cols_M1, "cols_M1");
    }

    /**
     * @brief Initializes workload parameters for matrix multiplication from
     * a list of existing workload parameters.
     * @param[in] w_params Workload parameters from which to initialize. These will
     * be copied to internal representation.
     * @throws std::out_of_range if \p w_params does not have, at least, 3 parameters.
     * @throws std::logic_error if the type of any of the required parameters is
     * incorrect.
     * @details Matrix multiplication workload requires, at least, 3 parameters as
     * specified by its definition. If any of the parameters is missing or in the
     * wrong format, a std::exception or derived type is thrown.
     */
    MatrixMultiply(const std::vector<hebench::APIBridge::WorkloadParam> &w_params) :
        Common(w_params)
    {
        validateParams();
    }
    /**
     * @brief Initializes workload parameters for matrix multiplication from
     * a list of existing workload parameters.
     * @param[in] w_params Workload parameters from which to initialize. These will
     * be copied to internal representation.
     * @throws std::out_of_range if \p w_params does not have, at least, 3 parameters.
     * @throws std::logic_error if the type of any of the required parameters is
     * incorrect.
     * @details Matrix multiplication workload requires, at least, 3 parameters as
     * specified by its definition. If any of the parameters is missing or in the
     * wrong format, a std::exception or derived type is thrown.
     */
    MatrixMultiply(const hebench::APIBridge::WorkloadParams &w_params) :
        Common(w_params)
    {
        validateParams();
    }

private:
    void validateParams() const
    {
        if (m_w_params.size() < MinRequiredParameters)
            throw std::out_of_range("Workload requires, at least, " + std::to_string(MinRequiredParameters) + " parameters.");
        for (std::size_t i = 0; i < MinRequiredParameters; ++i)
            if (m_w_params[i].data_type != hebench::APIBridge::WorkloadParamType::UInt64)
                throw std::logic_error("Data type for workload parameter " + std::to_string(i) + " must be WorkloadParamType::UInt64.");
    }
};

class VectorSize : public Common
{
public:
    enum : std::size_t
    {
        Index_N,
        MinRequiredParameters
    };

    /**
     * @brief Number elements in a vector.
     */
    std::uint64_t n() const { return get<std::uint64_t>(Index_N); }
    /**
     * @brief Number elements in a vector.
     */
    std::uint64_t &n() { return m_w_params.at(Index_N).u_param; }

public:
    /**
     * @brief Initializes a new object to represent workload parameters for a
     * workload requiring a vector size.
     * @param[in] _n Number elements in a vector.
     */
    VectorSize(std::uint64_t _n = 0) :
        Common(MinRequiredParameters)
    {
        this->set<std::uint64_t>(Index_N, _n, "n");
    }

    /**
     * @brief Initializes workload parameters for a workload requiring a vector size
     * from a list of existing workload parameters.
     * @param[in] w_params Workload parameters from which to initialize. These will
     * be copied to internal representation.
     * @throws std::out_of_range if \p w_params does not have, at least, 1 parameter.
     * @throws std::logic_error if the type of any of the required parameters is
     * incorrect.
     * @details Workloads of this type require, at least, 1 parameter as
     * specified by their definition. If any of the parameters is missing or in the
     * wrong format, a std::exception or derived type is thrown.
     */
    VectorSize(const std::vector<hebench::APIBridge::WorkloadParam> &w_params) :
        Common(w_params)
    {
        validateParams();
    }

    /**
     * @brief Initializes workload parameters for a workload requiring a vector size
     * from a list of existing workload parameters.
     * @param[in] w_params Workload parameters from which to initialize. These will
     * be copied to internal representation.
     * @throws std::out_of_range if \p w_params does not have, at least, 1 parameter.
     * @throws std::logic_error if the type of any of the required parameters is
     * incorrect.
     * @details Workloads of this type require, at least, 1 parameter as
     * specified by their definition. If any of the parameters is missing or in the
     * wrong format, a std::exception or derived type is thrown.
     */
    VectorSize(const hebench::APIBridge::WorkloadParams &w_params) :
        Common(w_params)
    {
        validateParams();
    }

private:
    void validateParams() const
    {
        if (m_w_params.size() < MinRequiredParameters)
            throw std::out_of_range("Workload requires, at least, " + std::to_string(MinRequiredParameters) + " parameters.");
        for (std::size_t i = 0; i < MinRequiredParameters; ++i)
            if (m_w_params[i].data_type != hebench::APIBridge::WorkloadParamType::UInt64)
                throw std::logic_error("Data type for workload parameter " + std::to_string(i) + " must be WorkloadParamType::UInt64.");
    }
};

/**
 * @brief Wraps around flexible workload parameters required for a
 * generic workload.
 */
class Generic : public Common
{
public:
    enum : std::size_t
    {
        Index_N,
        Index_M,
    };

    /**
     * @brief Number of inputs to the operation.
     */
    const std::uint64_t &n() const { return get<std::uint64_t>(Index_N); }
    /**
     * @brief Number of outputs from the operation.
     */
    const std::uint64_t &m() const { return get<std::uint64_t>(Index_M); }
    /**
     * @brief Number of components in input vector at specified input index.
     * @param[in] index Index of input component.
     */
    std::uint64_t length_InputParam(std::size_t index) const
    {
        if (index > n())
            throw std::out_of_range(std::string(__func__) + ": Index out of range; "
                                    + std::to_string(index) + ", expected less than " + std::to_string(n()) + ".");
        return get<std::uint64_t>(index + 2);
    }
    /**
     * @brief Number of elements in input vector at specified input index.
     * @param[in] index Index of input component.
     */
    std::uint64_t &length_InputParam(std::size_t index)
    {
        if (index >= n())
            throw std::out_of_range(std::string(__func__) + ": Index out of range; "
                                    + std::to_string(index) + ", expected less than " + std::to_string(n()) + ".");
        return m_w_params.at(index + 2).u_param;
    }
    /**
     * @brief Number of elements in output vector at specified result index.
     * @param[in] index Index of result component.
     */
    std::uint64_t length_ResultComponent(std::size_t index) const
    {
        if (index >= m())
            throw std::out_of_range(std::string(__func__) + ": Index out of range; "
                                    + std::to_string(index) + ", expected less than " + std::to_string(m()) + ".");
        return get<std::uint64_t>(index + n() + 2);
    }
    /**
     * @brief Number of elements in output vector at specified result index.
     * @param[in] index Index of result component.
     */
    std::uint64_t &length_ResultComponent(std::size_t index)
    {
        if (index >= m())
            throw std::out_of_range(std::string(__func__) + ": Index out of range; "
                                    + std::to_string(index) + ", expected less than " + std::to_string(m()) + ".");
        return m_w_params.at(index + n() + 2).u_param;
    }

public:
    /**
     * @brief Initializes a new object to represent workload parameters for a
     * generic workload.
     * @param[in] _n Number of inputs to the operation.
     * @param[in] _m Number of outputs for the operation.
     * @details
     * This constructor will generate workload parameters according to the specification
     * for generic workload. Since `ResultComponent` and `InputParam` depend on `m` and `n`
     * respectively, clients should utilize accessor methods provided in this class. Accessing
     * and changing workload parameters through the base `Generic` interface is discouraged
     * and should only be done for advanced processing: incorrectly modifying the workload
     * parameters may cause undefined results.
     */
    Generic(std::uint64_t _n = 1, std::uint64_t _m = 1) :
        Common(_n + _m + 2)
    {
        this->set<std::uint64_t>(Index_N, _n, "n");
        this->set<std::uint64_t>(Index_M, _m, "m");
        for (std::size_t i = 0; i < _n; ++i)
            set<std::uint64_t>(2 + i, 1, "length_InputParam" + std::to_string(i));
        for (std::size_t i = 0; i < _m; ++i)
            set<std::uint64_t>(2 + _n + i, 1, "length_ResultComponent" + std::to_string(i));
    }

    /**
     * @brief Initializes workload parameters for a generic workload from a list of
     * existing workload parameters.
     * @param[in] w_params Workload parameters from which to initialize. These will
     * be copied to internal representation.
     * @throws std::out_of_range if number of elements in \p w_params is not enough
     * to contain all required generic workload parameters.
     * @throws std::logic_error if the type of any of the required parameters is
     * incorrect.
     * @details Workloads of this type require, at least, `m + n + 2` parameters as
     * specified by their definition. If any of the parameters is missing or in the
     * wrong format, a std::exception or derived type is thrown.
     */
    Generic(const std::vector<hebench::APIBridge::WorkloadParam> &w_params) :
        Common(w_params)
    {
        validateParams();
    }

    /**
     * @brief Initializes workload parameters for a generic workload from a list of
     * existing workload parameters.
     * @param[in] w_params Workload parameters from which to initialize. These will
     * be copied to internal representation.
     * @throws std::out_of_range if number of elements in \p w_params is not enough
     * to contain all required generic workload parameters.
     * @throws std::logic_error if the type of any of the required parameters is
     * incorrect.
     * @details Workloads of this type require, at least, `m + n + 2` parameters as
     * specified by their definition. If any of the parameters is missing or in the
     * wrong format, a std::exception or derived type is thrown.
     */
    Generic(const hebench::APIBridge::WorkloadParams &w_params) :
        Common(w_params)
    {
        validateParams();
    }

private:
    void validateParams() const
    {
        const std::size_t MinRequiredParameters = n() + m() + 2;
        if (m_w_params.size() < MinRequiredParameters)
            throw std::out_of_range("Workload requires, at least, " + std::to_string(MinRequiredParameters) + " parameters.");
        for (std::size_t i = 0; i < MinRequiredParameters; ++i)
            if (m_w_params[i].data_type != hebench::APIBridge::WorkloadParamType::UInt64)
                throw std::logic_error("Data type for workload parameter " + std::to_string(i) + " must be WorkloadParamType::UInt64.");
    }
};

typedef VectorSize EltwiseAdd;
typedef VectorSize EltwiseMultiply;
typedef VectorSize DotProduct;
typedef VectorSize LogisticRegression;

} // namespace WorkloadParams
} // namespace cpp
} // namespace hebench

#endif // defined _HEBench_API_Bridge_WorkloadParamsCPP_H_7e5fa8c2415240ea93eff148ed73539b
