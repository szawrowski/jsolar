// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_PARSER_JSON_PARSER_H_
#define JSOLAR_PARSER_JSON_PARSER_H_

#include <cctype>
#include <stdexcept>
#include <string>

#include "jsolar/defs/char_hex.h"
#include "jsolar/defs/string_const.h"
#include "jsolar/parser/json_parse_error_type.h"
#include "jsolar/types/json.h"
#include "jsolar/utility/value_maker.h"

namespace jsolar {
namespace impl {

class JsonParser {
public:
  static JsonParser Parse(const std::string& json) {
    JsonParser parser{json};

    if (json.empty()) {
      parser.SetError(JsonParseErrorType::kInvalidJson);
      parser.SetErrorPosition(0);
      return parser;
    }
    const auto first = FindFirstNonSpace(json);
    const auto last = FindLastNonSpace(json);

    if (json[first] != GetSymbol(CharHex::OpeningCurlyBracket)) {
      parser.SetError(JsonParseErrorType::kInvalidJson);
      parser.SetErrorPosition(first);
      return parser;
    }
    if (json[last] != GetSymbol(CharHex::ClosingCurlyBracket)) {
      parser.SetError(JsonParseErrorType::kInvalidJson);
      parser.SetErrorPosition(last);
      return parser;
    }
    parser.data_ = parser.ParseValue();
    return parser;
  }

  [[nodiscard]] bool HasError() const {
    return error_ != JsonParseErrorType::kNoError;
  }

  [[nodiscard]] std::pair<JsonParseErrorType, size_t> GetError() const {
    return std::make_pair(error_, error_position_);
  }

  [[nodiscard]] Json GetData() const { return data_; }

private:
  JsonParser(const std::string& json)
      : error_{JsonParseErrorType::kNoError}, json_{json}, position_{} {}

  Json ParseValue() {
    SkipWhitespaces();

    if (position_ >= json_.size()) {
      return Json{};
    }
    switch (json_[position_]) {
      case GetSymbol(CharHex::DoubleQuote):
        return ParseString();
      case GetSymbol(CharHex::OpeningCurlyBracket):
        return ParseObject();
      case GetSymbol(CharHex::OpeningSquareBracket):
        return ParseArray();
      case GetSymbol(CharHex::LowercaseT):
      case GetSymbol(CharHex::LowercaseF):
        return ParseBoolean();
      case GetSymbol(CharHex::LowercaseN):
        return ParseNull();
      default:
        if (std::isdigit(json_[position_]) ||
            json_[position_] == GetSymbol(CharHex::Minus) ||
            json_[position_] == GetSymbol(CharHex::Plus)) {
          return ParseNumber();
        }
        SetError(JsonParseErrorType::kUnexpectedCharacter);
        SetErrorPosition(position_);
        return Json{};
    }
  }

  Json ParseObject() {
    Json result = MakeObject();
    Next();
    SkipWhitespaces();

    while (position_ < json_.size()) {
      SkipWhitespaces();

      if (json_[position_] == GetSymbol(CharHex::ClosingCurlyBracket)) {
        Next();
        return result;
      }
      if (json_[position_] == GetSymbol(CharHex::DoubleQuote)) {
        Next();
        const auto key_end =
            json_.find(GetSymbol(CharHex::DoubleQuote), position_);

        if (key_end == std::string::npos) {
          SetError(JsonParseErrorType::kUnterminatedString);
          SetErrorPosition(position_);
          return Json{};
        }
        std::string key = json_.substr(position_, key_end - position_);
        position_ = key_end + 1;

        SkipWhitespaces();
        if (json_[position_] != GetSymbol(CharHex::Colon)) {
          SetError(JsonParseErrorType::kMissingColon);
          SetErrorPosition(position_);
          return Json{};
        }
        Next();
        SkipWhitespaces();
        result[key] = ParseValue();

        SkipWhitespaces();
        if (json_[position_] == GetSymbol(CharHex::Comma)) {
          Next();
          SkipWhitespaces();

          if (json_[position_] == GetSymbol(CharHex::ClosingCurlyBracket)) {
            SetError(JsonParseErrorType::kTrailingComma);
            SetErrorPosition(position_);
            return Json{};
          }
        } else if (json_[position_] ==
                   GetSymbol(CharHex::ClosingCurlyBracket)) {
          Next();
          return result;
        } else {
          SetError(JsonParseErrorType::kUnexpectedCharacter);
          SetErrorPosition(position_);
          return Json{};
        }
      } else {
        SetError(JsonParseErrorType::kUnexpectedCharacter);
        SetErrorPosition(position_);
        return Json{};
      }
    }
    SetError(JsonParseErrorType::kUnterminatedObject);
    SetErrorPosition(position_);
    return Json{};
  }

  Json ParseArray() {
    Json result = MakeArray();
    Next();
    SkipWhitespaces();

    while (position_ < json_.size()) {
      SkipWhitespaces();

      if (json_[position_] == GetSymbol(CharHex::ClosingSquareBracket)) {
        Next();
        return result;
      }
      const Json value = ParseValue();
      result.Append(value);

      SkipWhitespaces();
      if (json_[position_] == GetSymbol(CharHex::Comma)) {
        Next();
        SkipWhitespaces();

        if (json_[position_] == GetSymbol(CharHex::ClosingSquareBracket)) {
          SetError(JsonParseErrorType::kTrailingComma);
          SetErrorPosition(position_);
          return Json{};
        }
      } else if (json_[position_] == GetSymbol(CharHex::ClosingSquareBracket)) {
        Next();
        return result;
      } else {
        SetError(JsonParseErrorType::kUnexpectedCharacter);
        error_position_ = position_;
        return Json{};
      }
    }
    SetError(JsonParseErrorType::kUnterminatedArray);
    SetErrorPosition(position_);
    return Json{};
  }

  Json ParseNull() {
    if (json_.compare(position_, 4, GetNullStr()) == 0) {
      Next(4);
      return MakeNull();
    }
    SetError(JsonParseErrorType::kInvalidValue);
    SetErrorPosition(position_);
    return Json{};
  }

  Json ParseString() {
    Next();

    std::ostringstream escaped_stream;

    while (position_ < json_.size() &&
           json_[position_] != GetSymbol(CharHex::DoubleQuote)) {
      if (json_[position_] == GetSymbol(CharHex::Backslash)) {
        Next();
        if (position_ >= json_.size()) {
          SetError(JsonParseErrorType::kUnterminatedString);
          SetErrorPosition(position_);
          return Json{};
        }
        if (json_[position_] == GetSymbol(CharHex::DoubleQuote)) {
          escaped_stream << GetSymbol(CharHex::DoubleQuote);
        } else if (json_[position_] == GetSymbol(CharHex::Backslash)) {
          escaped_stream << GetSymbol(CharHex::Backslash);
        } else if (json_[position_] == GetSymbol(CharHex::LowercaseB)) {
          escaped_stream << GetSymbol(CharHex::Beckspace);
        } else if (json_[position_] == GetSymbol(CharHex::LowercaseF)) {
          escaped_stream << GetSymbol(CharHex::FormFeed);
        } else if (json_[position_] == GetSymbol(CharHex::LowercaseN)) {
          escaped_stream << GetSymbol(CharHex::LineFeed);
        } else if (json_[position_] == GetSymbol(CharHex::LowercaseR)) {
          escaped_stream << GetSymbol(CharHex::CarriageReturn);
        } else if (json_[position_] == GetSymbol(CharHex::LowercaseT)) {
          escaped_stream << GetSymbol(CharHex::HorizontalTab);
          ;
        } else if (json_[position_] == GetSymbol(CharHex::LowercaseU)) {
          if (position_ + 4 >= json_.size()) {
            SetError(JsonParseErrorType::kInvalidEscapeSequence);
            SetErrorPosition(position_);
            return Json{};
          }
          std::string hex_code = json_.substr(position_ + 1, 4);
          try {
            const unsigned int code_point = std::stoul(hex_code, nullptr, 16);
            escaped_stream << static_cast<char>(code_point);
          } catch (const std::exception&) {
            SetError(JsonParseErrorType::kInvalidEscapeSequence);
            SetErrorPosition(position_);
            return Json{};
          }
          position_ += 4;
        } else {
          SetError(JsonParseErrorType::kInvalidEscapeSequence);
          SetErrorPosition(position_);
          return Json{};
        }
        Next();
      } else {
        escaped_stream << json_[position_];
      }
      Next();
    }
    if (json_[position_] == GetSymbol(CharHex::DoubleQuote)) {
      Next();
      return escaped_stream.str();
    }
    SetError(JsonParseErrorType::kUnterminatedString);
    SetErrorPosition(position_);
    return Json{};
  }

  Json ParseBoolean() {
    if (json_.compare(position_, 4, GetBooleanStr(true)) == 0) {
      Next(4);
      return true;
    }
    if (json_.compare(position_, 5, GetBooleanStr(false)) == 0) {
      Next(5);
      return false;
    }
    SetError(JsonParseErrorType::kInvalidValue);
    SetErrorPosition(position_);
    return Json{};
  }

  Json ParseNumber() {
    Json result;
    auto end_number = position_;
    while (end_number < json_.size() &&
           (std::isdigit(json_[end_number]) ||
            json_[end_number] == GetSymbol(CharHex::Point) ||
            json_[end_number] == GetSymbol(CharHex::LowercaseE) ||
            json_[end_number] == GetSymbol(CharHex::CapitalE) ||
            json_[end_number] == GetSymbol(CharHex::Minus) ||
            json_[end_number] == GetSymbol(CharHex::Plus))) {
      ++end_number;
    }
    const std::string number_str =
        json_.substr(position_, end_number - position_);
    try {
      if (number_str.find(GetSymbol(CharHex::Plus)) != std::string::npos ||
          number_str.find(GetSymbol(CharHex::LowercaseE)) !=
              std::string::npos ||
          number_str.find(GetSymbol(CharHex::CapitalE)) != std::string::npos) {
        result = std::stod(number_str);
      } else {
        result = std::stoll(number_str);
      }
    } catch (const std::out_of_range&) {
      SetError(JsonParseErrorType::kOutOfRangeError);
      SetErrorPosition(position_);
      return Json{};
    } catch (const std::exception&) {
      SetError(JsonParseErrorType::kInvalidNumber);
      SetErrorPosition(position_);
      return Json{};
    }
    position_ = end_number;
    return result;
  }

  void SkipWhitespaces() {
    while (position_ < json_.size() &&
           (json_[position_] == GetSymbol(CharHex::Space) ||
            json_[position_] == GetSymbol(CharHex::HorizontalTab) ||
            json_[position_] == GetSymbol(CharHex::LineFeed) ||
            json_[position_] == GetSymbol(CharHex::CarriageReturn))) {
      Next();
    }
  }

  void Next(const size_t count = 1) { position_ += count; }

  void SetError(const JsonParseErrorType error) { error_ = error; }

  void SetErrorPosition(const size_t position) { error_position_ = position; }

private:
  Json data_;
  JsonParseErrorType error_;
  const std::string& json_;
  size_t position_;
  size_t error_position_;
};

}  // namespace impl
}  // namespace jsolar

// Comment

#endif  // JSOLAR_PARSER_JSON_PARSER_H_