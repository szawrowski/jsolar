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

namespace cait {
namespace json {
namespace impl {

class json_parser_t {
public:
  using size_type = size_t;

public:
  static json_parser_t parse(const std::string& json) {
    json_parser_t parser{json};

    if (json.empty()) {
      parser.set_error(json_parse_error_type::invalid_json);
      parser.set_error_position(0);
      return parser;
    }
    const auto first = find_first_nonws(json);
    const auto last = find_last_nonws(json);

    if (json[first] != get_symbol(char_hex::opening_curly_bracket)) {
      parser.set_error(json_parse_error_type::invalid_json);
      parser.set_error_position(first);
      return parser;
    }
    if (json[last] != get_symbol(char_hex::closing_curly_bracket)) {
      parser.set_error(json_parse_error_type::invalid_json);
      parser.set_error_position(last);
      return parser;
    }
    parser.data_ = parser.parse_value();
    return parser;
  }

  [[nodiscard]] bool has_error() const {
    return error_ != json_parse_error_type::no_error;
  }

  [[nodiscard]] std::pair<json_parse_error_type, size_t> get_error() const {
    return std::make_pair(error_, error_position_);
  }

  [[nodiscard]] json_t get_data() const { return data_; }

private:
  json_parser_t(const std::string& json)
      : error_{json_parse_error_type::no_error}, json_{json}, position_{} {}

  json_t parse_value() {
    skip_ws();

    if (position_ >= json_.size()) {
      return json_t{};
    }
    switch (json_[position_]) {
      case get_symbol(char_hex::double_quote):
        return parse_string();
      case get_symbol(char_hex::opening_curly_bracket):
        return parse_object();
      case get_symbol(char_hex::opening_square_bracket):
        return parse_array();
      case get_symbol(char_hex::lowercase_t):
      case get_symbol(char_hex::lowercase_f):
        return parse_boolean();
      case get_symbol(char_hex::lowercase_n):
        return parse_null();
      default:
        if (std::isdigit(json_[position_]) ||
            json_[position_] == get_symbol(char_hex::minus) ||
            json_[position_] == get_symbol(char_hex::plus)) {
          return parse_number();
            }
      set_error(json_parse_error_type::unexpected_character);
      set_error_position(position_);
      return json_t{};
    }
  }

  json_t parse_object() {
    json_t result = make_object();
    next();
    skip_ws();

    while (position_ < json_.size()) {
      skip_ws();

      if (json_[position_] == get_symbol(char_hex::closing_curly_bracket)) {
        next();
        return result;
      }
      if (json_[position_] == get_symbol(char_hex::double_quote)) {
        next();
        const auto key_end =
            json_.find(get_symbol(char_hex::double_quote), position_);

        if (key_end == std::string::npos) {
          set_error(json_parse_error_type::unterminated_string);
          set_error_position(position_);
          return json_t{};
        }
        std::string key = json_.substr(position_, key_end - position_);
        position_ = key_end + 1;

        skip_ws();
        if (json_[position_] != get_symbol(char_hex::colon)) {
          set_error(json_parse_error_type::missing_colon);
          set_error_position(position_);
          return json_t{};
        }
        next();
        skip_ws();
        result[key] = parse_value();

        skip_ws();
        if (json_[position_] == get_symbol(char_hex::comma)) {
          next();
          skip_ws();

          if (json_[position_] == get_symbol(char_hex::closing_curly_bracket)) {
            set_error(json_parse_error_type::trailing_comma);
            set_error_position(position_);
            return json_t{};
          }
        } else if (json_[position_] ==
                   get_symbol(char_hex::closing_curly_bracket)) {
          next();
          return result;
                   } else {
                     set_error(json_parse_error_type::unexpected_character);
                     set_error_position(position_);
                     return json_t{};
                   }
      } else {
        set_error(json_parse_error_type::unexpected_character);
        set_error_position(position_);
        return json_t{};
      }
    }
    set_error(json_parse_error_type::unterminated_object);
    set_error_position(position_);
    return json_t{};
  }

  json_t parse_array() {
    json_t result = make_array();
    next();
    skip_ws();

    while (position_ < json_.size()) {
      skip_ws();

      if (json_[position_] == get_symbol(char_hex::closing_square_bracket)) {
        next();
        return result;
      }
      const json_t value = parse_value();
      result.append(value);

      skip_ws();
      if (json_[position_] == get_symbol(char_hex::comma)) {
        next();
        skip_ws();

        if (json_[position_] == get_symbol(char_hex::closing_square_bracket)) {
          set_error(json_parse_error_type::trailing_comma);
          set_error_position(position_);
          return json_t{};
        }
      } else if (json_[position_] ==
                 get_symbol(char_hex::closing_square_bracket)) {
        next();
        return result;
                 } else {
                   set_error(json_parse_error_type::unexpected_character);
                   error_position_ = position_;
                   return json_t{};
                 }
    }
    set_error(json_parse_error_type::unterminated_array);
    set_error_position(position_);
    return json_t{};
  }

  json_t parse_null() {
    if (json_.compare(position_, 4, get_null_str()) == 0) {
      next(4);
      return make_null();
    }
    set_error(json_parse_error_type::invalid_value);
    set_error_position(position_);
    return json_t{};
  }

  json_t parse_string() {
    next();
    std::ostringstream escaped_stream;

    while (position_ < json_.size() &&
           json_[position_] != get_symbol(char_hex::double_quote)) {
      if (json_[position_] == get_symbol(char_hex::backslash)) {
        next();
        if (position_ >= json_.size()) {
          set_error(json_parse_error_type::unterminated_string);
          set_error_position(position_);
          return json_t{};
        }
        if (json_[position_] == get_symbol(char_hex::double_quote)) {
          escaped_stream << get_symbol(char_hex::double_quote);
        } else if (json_[position_] == get_symbol(char_hex::backslash)) {
          escaped_stream << get_symbol(char_hex::backslash);
        } else if (json_[position_] == get_symbol(char_hex::lowercase_b)) {
          escaped_stream << get_symbol(char_hex::backspace);
        } else if (json_[position_] == get_symbol(char_hex::lowercase_f)) {
          escaped_stream << get_symbol(char_hex::form_feed);
        } else if (json_[position_] == get_symbol(char_hex::lowercase_n)) {
          escaped_stream << get_symbol(char_hex::line_feed);
        } else if (json_[position_] == get_symbol(char_hex::lowercase_r)) {
          escaped_stream << get_symbol(char_hex::carriage_return);
        } else if (json_[position_] == get_symbol(char_hex::lowercase_t)) {
          escaped_stream << get_symbol(char_hex::horizontal_tab);
          ;
        } else if (json_[position_] == get_symbol(char_hex::lowercase_u)) {
          if (position_ + 4 >= json_.size()) {
            set_error(json_parse_error_type::invalid_escape_sequence);
            set_error_position(position_);
            return json_t{};
          }
          std::string hex_code = json_.substr(position_ + 1, 4);
          try {
            const unsigned int code_point = std::stoul(hex_code, nullptr, 16);
            escaped_stream << static_cast<char>(code_point);
          } catch (const std::exception&) {
            set_error(json_parse_error_type::invalid_escape_sequence);
            set_error_position(position_);
            return json_t{};
          }
          position_ += 4;
        } else {
          set_error(json_parse_error_type::invalid_escape_sequence);
          set_error_position(position_);
          return json_t{};
        }
        next();
      } else {
        escaped_stream << json_[position_];
      }
      next();
           }
    if (json_[position_] == get_symbol(char_hex::double_quote)) {
      next();
      return escaped_stream.str();
    }
    set_error(json_parse_error_type::unterminated_string);
    set_error_position(position_);
    return json_t{};
  }

  json_t parse_boolean() {
    if (json_.compare(position_, 4, get_boolean_str(true)) == 0) {
      next(4);
      return true;
    }
    if (json_.compare(position_, 5, get_boolean_str(false)) == 0) {
      next(5);
      return false;
    }
    set_error(json_parse_error_type::invalid_value);
    set_error_position(position_);
    return json_t{};
  }

  json_t parse_number() {
    json_t result;
    auto end_number = position_;
    while (end_number < json_.size() &&
           (std::isdigit(json_[end_number]) ||
            json_[end_number] == get_symbol(char_hex::point) ||
            json_[end_number] == get_symbol(char_hex::lowercase_e) ||
            json_[end_number] == get_symbol(char_hex::capital_e) ||
            json_[end_number] == get_symbol(char_hex::minus) ||
            json_[end_number] == get_symbol(char_hex::plus))) {
      ++end_number;
            }
    const std::string number_str =
        json_.substr(position_, end_number - position_);
    try {
      if (number_str.find(get_symbol(char_hex::plus)) != std::string::npos ||
          number_str.find(get_symbol(char_hex::lowercase_e)) !=
              std::string::npos ||
          number_str.find(get_symbol(char_hex::capital_e)) !=
              std::string::npos) {
        result = std::stod(number_str);
              } else {
                result = std::stoll(number_str);
              }
    } catch (const std::out_of_range&) {
      set_error(json_parse_error_type::out_of_range_error);
      set_error_position(position_);
      return json_t{};
    } catch (const std::exception&) {
      set_error(json_parse_error_type::invalid_number);
      set_error_position(position_);
      return json_t{};
    }
    position_ = end_number;
    return result;
  }

  void skip_ws() {
    while (position_ < json_.size() &&
           (json_[position_] == get_symbol(char_hex::space) ||
            json_[position_] == get_symbol(char_hex::horizontal_tab) ||
            json_[position_] == get_symbol(char_hex::line_feed) ||
            json_[position_] == get_symbol(char_hex::carriage_return))) {
      next();
            }
  }

  void next(const size_t count = 1) { position_ += count; }

  void set_error(const json_parse_error_type error) { error_ = error; }

  void set_error_position(const size_t position) { error_position_ = position; }

private:
  json_t data_;
  json_parse_error_type error_;
  const std::string& json_;
  size_type position_;
  size_type error_position_;
};

}  // namespace impl
}  // namespace json
}  // namespace cait

// Comment

#endif  // JSOLAR_PARSER_JSON_PARSER_H_