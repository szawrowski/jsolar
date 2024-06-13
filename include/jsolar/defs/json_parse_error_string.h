// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_DEFS_JSON_PARSE_ERROR_STRING_H_
#define JSOLAR_DEFS_JSON_PARSE_ERROR_STRING_H_

#include <sstream>

#include "jsolar/parser/json_parse_error_type.h"

namespace jsolar {
namespace impl {

static std::string GetJsonParseErrorString(
    const std::pair<JsonParseErrorType, size_t>& error) {
  std::ostringstream oss;
  const auto position_header{". At position: "};

  switch (error.first) {
    case JsonParseErrorType::kNoError:
      oss << "No error";
      return oss.str();
    case JsonParseErrorType::kUnterminatedString:
      oss << "Unterminated string";
      break;
    case JsonParseErrorType::kExpectedStringKey:
      oss << "Expected string key";
      break;
    case JsonParseErrorType::kMissingColon:
      oss << "Missing colon";
      break;
    case JsonParseErrorType::kUnterminatedObject:
      oss << "Unterminated object";
      break;
    case JsonParseErrorType::kUnterminatedArray:
      oss << "Unterminated array";
      break;
    case JsonParseErrorType::kInvalidNumber:
      oss << "Invalid number";
      break;
    case JsonParseErrorType::kNumberConversionError:
      oss << "Number conversion error";
      break;
    case JsonParseErrorType::kInvalidValue:
      oss << "Invalid value";
      break;
    case JsonParseErrorType::kUnexpectedCharacter:
      oss << "Unexpected character";
      break;
    case JsonParseErrorType::kTrailingComma:
      oss << "Trailing comma";
      break;
    case JsonParseErrorType::kInvalidJson:
      oss << "Invalid JSON";
      break;
    default:
      oss << "Unknown error";
      return oss.str();
  }
  oss << position_header << error.second;
  return oss.str();
}

}  // namespace impl
}  // namespace jsolar

#endif  // JSOLAR_DEFS_JSON_PARSE_ERROR_STRING_H_
