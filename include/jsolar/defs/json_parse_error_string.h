// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_DEFS_JSON_PARSE_ERROR_STRING_H_
#define JSOLAR_DEFS_JSON_PARSE_ERROR_STRING_H_

#include <sstream>

#include "jsolar/parser/json_parse_error_type.h"

namespace json {
namespace impl {

static std::string get_json_parse_error_string(
    const std::pair<json_parse_error_type, size_t>& error) {
  std::ostringstream oss;
  const auto position_header{" (at position: )"};

  switch (error.first) {
    case json_parse_error_type::no_error:
      oss << "No error";
      return oss.str();
    case json_parse_error_type::unterminated_string:
      oss << "Unterminated string";
      break;
    case json_parse_error_type::expected_string_key:
      oss << "Expected string key";
      break;
    case json_parse_error_type::missing_colon:
      oss << "Missing colon";
      break;
    case json_parse_error_type::unterminated_object:
      oss << "Unterminated object";
      break;
    case json_parse_error_type::unterminated_array:
      oss << "Unterminated array";
      break;
    case json_parse_error_type::invalid_number:
      oss << "Invalid number";
      break;
    case json_parse_error_type::number_conversion_error:
      oss << "Number conversion error";
      break;
    case json_parse_error_type::invalid_value:
      oss << "Invalid value";
      break;
    case json_parse_error_type::unexpected_character:
      oss << "Unexpected character";
      break;
    case json_parse_error_type::trailing_comma:
      oss << "Trailing comma";
      break;
    case json_parse_error_type::invalid_json:
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
}  // namespace json

#endif  // JSOLAR_DEFS_JSON_PARSE_ERROR_STRING_H_
