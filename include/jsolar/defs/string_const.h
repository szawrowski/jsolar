// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_DEFS_STRING_CONST_H_
#define JSOLAR_DEFS_STRING_CONST_H_

namespace jsolar {
namespace impl {

static constexpr auto GetBooleanStr(const bool value) {
  return value ? "true" : "false";
}

static constexpr auto GetNullStr() { return "null"; }

}  // namespace impl
}  // namespace jsolar

#endif  // JSOLAR_DEFS_STRING_CONST_H_
