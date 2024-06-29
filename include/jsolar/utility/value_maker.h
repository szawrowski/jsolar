// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_UTIL_VALUE_MAKER_H_
#define JSOLAR_UTIL_VALUE_MAKER_H_

#include "jsolar/types/json.h"

namespace json {

static impl::json_t make(const json_class_type value) {
  return impl::json_t::__internal_make(value);
}

static impl::json_t make_object() { return make(json_class_type::object); }

static impl::json_t make_array() { return make(json_class_type::array); }

template <typename... T>
static impl::json_t make_array(T... args) {
  auto array = make(json_class_type::array);
  array.append(args...);
  return array;
}

static impl::json_t make_null() { return make(json_class_type::null); }

static impl::json_t make_string(const impl::json_t::string_type& value) {
  return impl::json_t{value};
}

static impl::json_t make_floating(const impl::json_t::floating_type value) {
  return impl::json_t{value};
}

static impl::json_t make_integral(const impl::json_t::integral_type value) {
  return impl::json_t{value};
}

static impl::json_t make_boolean(const impl::json_t::boolean_type value) {
  return impl::json_t{value};
}

}  // namespace json

#endif  // JSOLAR_UTIL_VALUE_MAKER_H_
