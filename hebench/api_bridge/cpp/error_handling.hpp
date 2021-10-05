
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_Error_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_Error_H_7e5fa8c2415240ea93eff148ed73539b

#include <stdexcept>
#include <string>

namespace hebench {
namespace cpp {

#define HEBERROR_DECLARE_CLASS_NAME(class_name) static constexpr const char *m_private_class_name = #class_name;

#define HEBERROR_MSG_CLASS(message) hebench::cpp::HEBenchError::generateMessage((message),                      \
                                                                                __func__, m_private_class_name, \
                                                                                __FILE__, __LINE__)

#define HEBERROR_MSG(message) hebench::cpp::HEBenchError::generateMessage((message),               \
                                                                          __func__, std::string(), \
                                                                          __FILE__, __LINE__)

class HEBenchError : public std::runtime_error
{
public:
    HEBenchError(const std::string &msg = std::string(), int err_code = 0) :
        std::runtime_error(msg), m_err_code(err_code)
    {
    }

    int getErrorCode() const { return m_err_code; }

    static std::string generateMessage(const std::string &message,
                                       const std::string &function,
                                       const std::string &container,
                                       const std::string &filename,
                                       int line_no);

private:
    int m_err_code;
};

} // namespace cpp
} // namespace hebench

#endif // defined _HEBench_API_Bridge_Error_H_7e5fa8c2415240ea93eff148ed73539b
