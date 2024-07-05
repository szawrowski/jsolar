// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_DEFS_STRING_CONST_H_
#define JSOLAR_DEFS_STRING_CONST_H_

namespace cait {
namespace json {
namespace impl {

static constexpr auto get_boolean_str(const bool value) {
  return value ? "true" : "false";
}

static constexpr auto get_null_str() { return "null"; }

}  // namespace impl
}  // namespace json
}  // namespace cait

#endif  // JSOLAR_DEFS_STRING_CONST_H_
