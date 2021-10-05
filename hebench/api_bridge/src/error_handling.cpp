
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <sstream>

#include "hebench/api_bridge/cpp/error_handling.hpp"

namespace hebench {
namespace cpp {

std::string HEBenchError::generateMessage(const std::string &message,
                                          const std::string &function,
                                          const std::string &container,
                                          const std::string &filename,
                                          int line_no)
{
    std::stringstream ss_retval;
    // prepare message
    bool bheader = (!filename.empty() || !container.empty() || !function.empty() || line_no >= 0);
    if (!filename.empty())
        ss_retval << filename << ":";
    if (line_no >= 0)
        ss_retval << line_no << ":";
    if (!container.empty())
    {
        ss_retval << container;
        if (!function.empty())
            ss_retval << "::";
    } // end if
    if (!function.empty())
        ss_retval << function << "()";
    if (!message.empty())
    {
        if (bheader)
            ss_retval << ": ";
        ss_retval << message;
    } // end if

    return ss_retval.str();
}

} // namespace cpp
} // namespace hebench
