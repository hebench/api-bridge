
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>

#include "hebench/api_bridge/cpp/utilities.hpp"

namespace hebench {
namespace cpp {
namespace Utilities {

std::uint64_t copyString(char *dst, std::uint64_t size, const std::string &src)
{
    std::uint64_t retval = src.size() + 1;

    if (dst && size > 0)
    {
        std::uint64_t min_size = std::min(size, retval);
        if (min_size > 1)
            std::copy_n(src.c_str(), min_size - 1, dst);
        dst[min_size - 1] = '\0'; // close string
    } // end if

    return retval;
}

} // namespace Utilities
} // namespace cpp
} // namespace hebench
