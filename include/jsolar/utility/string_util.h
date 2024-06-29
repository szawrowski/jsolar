// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_UTIL_STRING_UTIL_H_
#define JSOLAR_UTIL_STRING_UTIL_H_

#include <sstream>
#include <string>

#include "jsolar/defs/char_hex.h"

namespace json {
namespace impl {

static auto escape_string(const std::string& str) {
  std::ostringstream oss;

  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == get_symbol(char_hex::backslash) && i + 1 < str.size()) {
      switch (str[i + 1]) {
        case get_symbol(char_hex::double_quote):
          oss << get_symbol(char_hex::double_quote);
          break;
        case get_symbol(char_hex::question_mark):
          oss << get_symbol(char_hex::question_mark);
          break;
        case get_symbol(char_hex::backslash):
          oss << get_symbol(char_hex::backslash);
          break;
        case get_symbol(char_hex::slash):
          oss << get_symbol(char_hex::slash);
          break;
        case get_symbol(char_hex::lowercase_a):
          oss << get_symbol(char_hex::audible_bell);
          break;
        case get_symbol(char_hex::lowercase_b):
          oss << get_symbol(char_hex::backspace);
          break;
        case get_symbol(char_hex::lowercase_f):
          oss << get_symbol(char_hex::form_feed);
          break;
        case get_symbol(char_hex::lowercase_n):
          oss << get_symbol(char_hex::line_feed);
          break;
        case get_symbol(char_hex::lowercase_r):
          oss << get_symbol(char_hex::carriage_return);
          break;
        case get_symbol(char_hex::lowercase_t):
          oss << get_symbol(char_hex::horizontal_tab);
          break;
        case get_symbol(char_hex::lowercase_v):
          oss << get_symbol(char_hex::vertical_tab);
          break;
        case get_symbol(char_hex::lowercase_u): {
          if (i + 5 < str.size()) {
            auto hex = str.substr(i + 2, 4);
            // Convert the hexadecimal representation to a character
            const char c = static_cast<char>(std::stoi(hex, nullptr, 16));
            oss << c;
            i += 5;  // Skip the escaped sequence
          } else {
            // Incomplete escape sequence, handle it as an error
            oss << get_symbol(char_hex::backslash) + str[i + 1];
          }
          break;
        }
        default:
          // Invalid escape sequence, handle it as an error
          oss << get_symbol(char_hex::backslash) + str[i + 1];
          break;
      }
      ++i;  // Skip the escaped character
    } else {
      oss << str[i];
    }
  }
  return oss.str();
}

static auto is_number(const std::string& s) {
  return !s.empty() &&
         std::find_if(s.begin(), s.end(), [](const unsigned char c) {
           return !std::isdigit(c);
         }) == s.end();
}

static size_t find_first_nonws(const std::string& str) {
  for (size_t i = 0; i < str.size(); ++i) {
    if (!std::isspace(str[i])) {
      return i;
    }
  }
  return str.size();
}

static size_t find_last_nonws(const std::string& str) {
  for (size_t i = str.size() - 1; i > 0; --i) {
    if (!std::isspace(str[i])) {
      return i;
    }
  }
  return str.size();
}

}  // namespace impl
}  // namespace json

#endif  // JSOLAR_UTIL_STRING_UTIL_H_
