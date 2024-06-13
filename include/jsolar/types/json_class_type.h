// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_TYPES_JSON_CLASS_TYPE_H_
#define JSOLAR_TYPES_JSON_CLASS_TYPE_H_

namespace jsolar {

enum class JsonClassType {
  kNull,
  kObject,
  kArray,
  kString,
  kFloating,
  kIntegral,
  kBoolean
};

}  // namespace jsolar

#endif  // JSOLAR_TYPES_JSON_CLASS_TYPE_H_
