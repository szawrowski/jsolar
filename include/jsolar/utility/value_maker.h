// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_UTIL_VALUE_MAKER_H_
#define JSOLAR_UTIL_VALUE_MAKER_H_

#include "jsolar/types/json.h"

namespace jsolar {

static impl::Json Make(const JsonClassType value) {
  return impl::Json::internal_make(value);
}

static impl::Json MakeObject() { return Make(JsonClassType::kObject); }

static impl::Json MakeArray() { return Make(JsonClassType::kArray); }

template <typename... T>
static impl::Json MakeArray(T... args) {
  auto array = Make(JsonClassType::kArray);
  array.Append(args...);
  return array;
}

static impl::Json MakeNull() { return Make(JsonClassType::kNull); }

static impl::Json MakeString(const impl::Json::StringType& value) {
  return impl::Json{value};
}

static impl::Json MakeFloating(const impl::Json::FloatingType value) {
  return impl::Json{value};
}

static impl::Json MakeFloating(const impl::Json::IntegralType value) {
  return impl::Json{value};
}

static impl::Json MakeFloating(const impl::Json::BooleanType value) {
  return impl::Json{value};
}

}  // namespace jsolar

#endif  // JSOLAR_UTIL_VALUE_MAKER_H_
