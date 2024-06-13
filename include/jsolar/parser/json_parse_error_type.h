// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_PARSER_JSON_PARSE_ERROR_TYPE_H_
#define JSOLAR_PARSER_JSON_PARSE_ERROR_TYPE_H_

namespace jsolar {

enum class JsonParseErrorType {
  kNoError,
  kUnterminatedString,
  kExpectedStringKey,
  kMissingColon,
  kUnterminatedObject,
  kUnterminatedArray,
  kInvalidNumber,
  kNumberConversionError,
  kInvalidValue,
  kUnexpectedCharacter,
  kOutOfRangeError,
  kInvalidEscapeSequence,
  kTrailingComma,
  kInvalidJson
};

}  // namespace jsolar

#endif  // JSOLAR_PARSER_JSON_PARSE_ERROR_TYPE_H_
