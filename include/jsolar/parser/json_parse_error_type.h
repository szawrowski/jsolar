// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_PARSER_JSON_PARSE_ERROR_TYPE_H_
#define JSOLAR_PARSER_JSON_PARSE_ERROR_TYPE_H_

namespace json {

enum class json_parse_error_type {
  no_error,
  unterminated_string,
  expected_string_key,
  missing_colon,
  unterminated_object,
  unterminated_array,
  invalid_number,
  number_conversion_error,
  invalid_value,
  unexpected_character,
  out_of_range_error,
  invalid_escape_sequence,
  trailing_comma,
  invalid_json
};

}  // namespace json

#endif  // JSOLAR_PARSER_JSON_PARSE_ERROR_TYPE_H_
