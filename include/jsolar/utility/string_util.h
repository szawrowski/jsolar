// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_UTIL_STRING_UTIL_H_
#define JSOLAR_UTIL_STRING_UTIL_H_

#include <sstream>
#include <string>

#include "jsolar/defs/char_hex.h"

namespace jsolar {
namespace impl {

static auto EscapeString(const std::string& str) {
  std::ostringstream oss;

  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == GetSymbol(CharHex::Backslash) && i + 1 < str.size()) {
      switch (str[i + 1]) {
        case GetSymbol(CharHex::DoubleQuote):
          oss << GetSymbol(CharHex::DoubleQuote);
          break;
        case GetSymbol(CharHex::QuestionMark):
          oss << GetSymbol(CharHex::QuestionMark);
          break;
        case GetSymbol(CharHex::Backslash):
          oss << GetSymbol(CharHex::Backslash);
          break;
        case GetSymbol(CharHex::Slash):
          oss << GetSymbol(CharHex::Slash);
          break;
        case GetSymbol(CharHex::LowercaseA):
          oss << GetSymbol(CharHex::AudibleBell);
          break;
        case GetSymbol(CharHex::LowercaseB):
          oss << GetSymbol(CharHex::Beckspace);
          break;
        case GetSymbol(CharHex::LowercaseF):
          oss << GetSymbol(CharHex::FormFeed);
          break;
        case GetSymbol(CharHex::LowercaseN):
          oss << GetSymbol(CharHex::LineFeed);
          break;
        case GetSymbol(CharHex::LowercaseR):
          oss << GetSymbol(CharHex::CarriageReturn);
          break;
        case GetSymbol(CharHex::LowercaseT):
          oss << GetSymbol(CharHex::HorizontalTab);
          break;
        case GetSymbol(CharHex::LowercaseV):
          oss << GetSymbol(CharHex::VerticalTab);
          break;
        case GetSymbol(CharHex::LowercaseU): {
          if (i + 5 < str.size()) {
            auto hex = str.substr(i + 2, 4);
            // Convert the hexadecimal representation to a character
            const char c = static_cast<char>(std::stoi(hex, nullptr, 16));
            oss << c;
            i += 5;  // Skip the escaped sequence
          } else {
            // Incomplete escape sequence, handle it as an error
            oss << GetSymbol(CharHex::Backslash) + str[i + 1];
          }
          break;
        }
        default:
          // Invalid escape sequence, handle it as an error
          oss << GetSymbol(CharHex::Backslash) + str[i + 1];
          break;
      }
      ++i;  // Skip the escaped character
    } else {
      oss << str[i];
    }
  }
  return oss.str();
}

static auto IsNumber(const std::string& s) {
  return !s.empty() &&
         std::find_if(s.begin(), s.end(), [](const unsigned char c) {
           return !std::isdigit(c);
         }) == s.end();
}

static size_t FindFirstNonSpace(const std::string& str) {
  for (size_t i = 0; i < str.size(); ++i) {
    if (!std::isspace(str[i])) {
      return i;
    }
  }
  return str.size();
}

static size_t FindLastNonSpace(const std::string& str) {
  for (size_t i = str.size() - 1; i > 0; --i) {
    if (!std::isspace(str[i])) {
      return i;
    }
  }
  return str.size();
}

}  // namespace impl

static auto GetIndex(const size_t index) { return std::to_string(index); }

}  // namespace jsolar

#endif  // JSOLAR_UTIL_STRING_UTIL_H_
