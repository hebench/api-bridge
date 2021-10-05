
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_UtilitiesCPP_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_UtilitiesCPP_H_7e5fa8c2415240ea93eff148ed73539b

#include <cstdint>
#include <string>

namespace hebench {
namespace cpp {
namespace Utilities {

/**
 * @brief Copies a C++ string object into a C-style string.
 * @param[in] dst Points to the destination array of characters. Can be null.
 * @param[in] size Number of characters in array pointed to by \p dst.
 * @param[in] src String to copy.
 * @return The number of characters required to copy \src completely into
 * \p dst, including the C-style null terminator.
 * @details If \p dst is null, no copy is performed. Use this behavior to
 * obtain information regarding the size required to hold \p src string.
 */
std::uint64_t copyString(char *dst, std::uint64_t size, const std::string &src);

} // namespace Utilities
} // namespace cpp
} // namespace hebench

#endif // defined _HEBench_API_Bridge_UtilitiesCPP_H_7e5fa8c2415240ea93eff148ed73539b
