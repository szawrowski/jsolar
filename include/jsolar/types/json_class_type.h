// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_TYPES_JSON_CLASS_TYPE_H_
#define JSOLAR_TYPES_JSON_CLASS_TYPE_H_

namespace cait {
namespace json {

enum class json_class_type {
  null,
  object,
  array,
  string,
  floating,
  integral,
  boolean
};

}  // namespace json
}  // namespace cait

#endif  // JSOLAR_TYPES_JSON_CLASS_TYPE_H_
