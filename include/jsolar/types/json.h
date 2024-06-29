// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_TYPES_JSON_H_
#define JSOLAR_TYPES_JSON_H_

#include <algorithm>
#include <deque>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "jsolar/types/json_class_type.h"
#include "jsolar/utility/string_util.h"

namespace json {
namespace impl {

class json_t {
public:
  using array_type = std::deque<json_t*>;
  using object_type = std::vector<std::pair<std::string, json_t*>>;
  using string_type = std::string;
  using null_type = std::nullptr_t;
  using floating_type = double;
  using integral_type = long long;
  using boolean_type = bool;

  using data_type =
      std::variant<null_type, object_type, array_type, string_type,
                   floating_type, integral_type, boolean_type>;

public:
  json_t() = default;

  json_t(const json_t& other) : data_{copy_data(other)}, type_{other.type_} {}

  json_t(json_t&& other) noexcept
      : data_{std::move(other.data_)}, type_{other.type_} {
    other.data_ = null_type{};
    other.type_ = json_class_type::null;
  }

  template <typename T>
  json_t(T value, std::enable_if_t<std::is_same_v<T, boolean_type>>* = nullptr)
      : data_{value}, type_{json_class_type::boolean} {}

  template <typename T>
  json_t(T value, std::enable_if_t<std::is_integral_v<T> &&
                                   !std::is_same_v<T, boolean_type>>* = nullptr)
      : data_{static_cast<integral_type>(value)},
        type_{json_class_type::integral} {}

  template <typename T>
  json_t(T value, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr)
      : data_{static_cast<floating_type>(value)},
        type_{json_class_type::floating} {}

  template <typename T>
  json_t(T value,
         std::enable_if_t<std::is_convertible_v<T, string_type>>* = nullptr)
      : data_{std::string{value}}, type_{json_class_type::string} {}

  ~json_t() { clear_internal(); }

public:
  json_t& operator=(const json_t& other) {
    if (this != &other) {
      clear_internal();
      data_ = copy_data(other);
      type_ = other.type_;
    }
    return *this;
  }

  json_t& operator=(json_t&& other) noexcept {
    if (this != &other) {
      clear_internal();
      data_ = std::move(other.data_);
      type_ = other.type_;
      other.data_ = null_type{};
      other.type_ = json_class_type::null;
    }
    return *this;
  }

  template <typename T>
  std::enable_if_t<std::is_same_v<T, boolean_type>, json_t&> operator=(
      T value) {
    set_type(json_class_type::boolean);
    data_ = value;
    return *this;
  }

  template <typename T>
  std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, boolean_type>,
                   json_t&>
  operator=(T value) {
    set_type(json_class_type::integral);
    data_ = static_cast<integral_type>(value);
    return *this;
  }

  template <typename T>
  std::enable_if_t<std::is_floating_point_v<T>, json_t&> operator=(T value) {
    set_type(json_class_type::floating);
    data_ = static_cast<floating_type>(value);
    return *this;
  }

  template <typename T>
  std::enable_if_t<std::is_convertible_v<T, string_type>, json_t&> operator=(
      T value) {
    set_type(json_class_type::string);
    data_ = std::string{value};
    return *this;
  }

public:
  json_t& operator[](const std::string& key) {
    set_type(json_class_type::object);
    const auto object = std::get_if<object_type>(&data_);
    const auto it =
        std::find_if(object->begin(), object->end(),
                     [&key](const auto& pair) { return pair.first == key; });
    if (it == object->end()) {
      object->emplace_back(key, new json_t{});
      return *object->back().second;
    }
    return *it->second;
  }

  json_t& operator[](const size_t index) {
    set_type(json_class_type::array);
    const auto array = std::get_if<array_type>(&data_);
    if (index >= array->size()) {
      array->resize(index + 1, nullptr);
    }
    if (!array->at(index)) {
      new (array->at(index)) json_t;
    }
    return *array->at(index);
  }

public:
  json_t& at(const std::string& key) { return operator[](key); }

  [[nodiscard]] const json_t& at(const std::string& key) const {
    const auto object = std::get_if<object_type>(&data_);
    const auto it =
        std::find_if(object->begin(), object->end(),
                     [&key](const auto& pair) { return pair.first == key; });
    if (it == object->end()) {
      throw std::out_of_range{"Key not found"};
    }
    return *it->second;
  }

  json_t& at(const size_t index) { return operator[](index); }

  [[nodiscard]] const json_t& at(const size_t index) const {
    return *std::get_if<array_type>(&data_)->at(index);
  }

public:
  template <typename T>
  void append(T arg) {
    if (!is_array()) {
      set_type(json_class_type::array);
    }
    std::get_if<array_type>(&data_)->emplace_back(new json_t{arg});
  }

  template <typename T, typename... U>
  void append(T arg, U... args) {
    append(arg);
    append(args...);
  }

public:
  [[nodiscard]] size_t size() const {
    if (is_array()) {
      return std::get_if<array_type>(&data_)->size();
    }
    if (is_object()) {
      return std::get_if<object_type>(&data_)->size();
    }
    return std::numeric_limits<size_t>::max();
  }

  [[nodiscard]] size_t length() const {
    if (is_array()) {
      return std::get_if<array_type>(&data_)->size();
    }
    return std::numeric_limits<size_t>::max();
  }

public:
  [[nodiscard]] bool has_member(const std::string& key) const {
    if (is_object()) {
      const auto& object = std::get_if<object_type>(&data_);
      return std::find_if(object->begin(), object->end(),
                          [&key](const auto& pair) {
                            return pair.first == key;
                          }) != object->end();
    }
    return false;
  }

  [[nodiscard]] bool is_null() const { return type_ == json_class_type::null; }

  [[nodiscard]] bool is_object() const {
    return type_ == json_class_type::object;
  }

  [[nodiscard]] bool is_array() const {
    return type_ == json_class_type::array;
  }

  [[nodiscard]] bool is_string() const {
    return type_ == json_class_type::string;
  }

  [[nodiscard]] bool is_floating() const {
    return type_ == json_class_type::floating;
  }

  [[nodiscard]] bool is_integral() const {
    return type_ == json_class_type::integral;
  }

  [[nodiscard]] bool is_boolean() const {
    return type_ == json_class_type::boolean;
  }

public:
  object_type& get_data() {
    if (type_ != json_class_type::object) {
      throw std::logic_error{"Trying to access non-object types as object"};
    }
    return *std::get_if<object_type>(&data_);
  }

  [[nodiscard]] json_class_type get_type() const { return type_; }

  [[nodiscard]] string_type get_string() const {
    return is_string() ? escape_string(*std::get_if<string_type>(&data_))
                       : string_type{};
  }

  [[nodiscard]] floating_type get_floating() const {
    return is_floating() ? *std::get_if<floating_type>(&data_)
                         : floating_type{};
  }

  [[nodiscard]] integral_type get_integral() const {
    return is_integral() ? *std::get_if<integral_type>(&data_)
                         : integral_type{};
  }

  [[nodiscard]] boolean_type get_boolean() const {
    return is_boolean() && *std::get_if<boolean_type>(&data_);
  }

public:
  [[nodiscard]] std::string to_string(const bool mangling = false,
                                      const size_t indent = 2) const {
    std::ostringstream oss;
    make_output(oss, mangling, indent, indent);
    return oss.str();
  }

  static json_t __internal_make(const json_class_type type) {
    json_t obj;
    obj.set_type(type);
    return obj;
  }

private:
  void make_output(std::ostringstream& oss, const bool mangling,
                   const size_t base_indent, const size_t indent) const {
    const std::string indent_str(indent, get_symbol(char_hex::space));

    switch (type_) {
      case json_class_type::object: {
        oss << get_symbol(char_hex::opening_curly_bracket);
        const auto data = std::get_if<object_type>(&data_);
        if (data && !data->empty()) {
          if (mangling) {
            oss << get_symbol(char_hex::line_feed);
          }
          bool first = true;
          for (const auto& [key, value] : *data) {
            if (!first) {
              oss << get_symbol(char_hex::comma);
              if (mangling) {
                oss << get_symbol(char_hex::line_feed);
              }
            }
            if (mangling) {
              oss << indent_str;
            }
            oss << get_symbol(char_hex::double_quote) << escape_string(key)
                << get_symbol(char_hex::double_quote)
                << get_symbol(char_hex::colon) << get_symbol(char_hex::space);
            value->make_output(oss, mangling, base_indent,
                               indent + base_indent);
            first = false;
          }
          if (mangling) {
            oss << get_symbol(char_hex::line_feed)
                << std::string(indent - base_indent,
                               get_symbol(char_hex::space));
          }
        }
        oss << get_symbol(char_hex::closing_curly_bracket);
        break;
      }
      case json_class_type::array: {
        oss << get_symbol(char_hex::opening_square_bracket);
        const auto data = std::get_if<array_type>(&data_);
        if (data && !data->empty()) {
          if (mangling) {
            oss << get_symbol(char_hex::line_feed);
          }
          bool first = true;
          for (const auto& item : *data) {
            if (!first) {
              oss << get_symbol(char_hex::comma);
              if (mangling) {
                oss << get_symbol(char_hex::line_feed);
              }
            }
            if (mangling) {
              oss << indent_str;
            }
            item->make_output(oss, mangling, base_indent, indent + base_indent);
            first = false;
          }
          if (mangling) {
            oss << get_symbol(char_hex::line_feed)
                << std::string(indent - base_indent,
                               get_symbol(char_hex::space));
          }
        }
        oss << get_symbol(char_hex::closing_square_bracket);
        break;
      }
      case json_class_type::null:
        oss << get_null_str();
        break;
      case json_class_type::string:
        oss << get_symbol(char_hex::double_quote)
            << escape_string(*std::get_if<string_type>(&data_))
            << get_symbol(char_hex::double_quote);
        break;
      case json_class_type::floating:
        oss << *std::get_if<floating_type>(&data_);
        break;
      case json_class_type::integral:
        oss << *std::get_if<integral_type>(&data_);
        break;
      case json_class_type::boolean:
        oss << (*std::get_if<boolean_type>(&data_) ? get_boolean_str(true)
                                                   : get_boolean_str(false));
        break;
    }
  }

  void set_type(const json_class_type type) {
    if (type_ != type) {
      clear_internal();
      type_ = type;
      switch (type_) {
        case json_class_type::null:
          data_ = null_type{};
          break;
        case json_class_type::object:
          data_ = object_type{};
          break;
        case json_class_type::array:
          data_ = array_type{};
          break;
        case json_class_type::string:
          data_ = string_type{};
          break;
        case json_class_type::floating:
          data_ = floating_type{};
          break;
        case json_class_type::integral:
          data_ = integral_type{};
          break;
        case json_class_type::boolean:
          data_ = boolean_type{};
          break;
      }
    }
  }

  void clear_internal() {
    switch (type_) {
      case json_class_type::object: {
        auto& object = *std::get_if<object_type>(&data_);
        for (auto& it : object) {
          delete it.second;
        }
        object.clear();
        break;
      }
      case json_class_type::array: {
        auto& array = *std::get_if<array_type>(&data_);
        for (auto* item : array) {
          delete item;
        }
        array.clear();
        break;
      }
      case json_class_type::string:
        std::get_if<string_type>(&data_)->clear();
        break;
      default:
        type_ = json_class_type::null;
    }
  }

  static data_type copy_data(const json_t& other) {
    switch (other.type_) {
      case json_class_type::object: {
        auto object = object_type{};
        const auto other_object = std::get_if<object_type>(&other.data_);
        for (const auto& [key, value] : *other_object) {
          object.emplace_back(key, new json_t{*value});
        }
        return object;
      }
      case json_class_type::array: {
        auto array = array_type{};
        const auto other_array = std::get_if<array_type>(&other.data_);
        for (const auto& item : *other_array) {
          array.emplace_back(new json_t{*item});
        }
        return array;
      }
      case json_class_type::null:
        return *std::get_if<null_type>(&other.data_);
      case json_class_type::string:
        return *std::get_if<string_type>(&other.data_);
      case json_class_type::floating:
        return *std::get_if<floating_type>(&other.data_);
      case json_class_type::integral:
        return *std::get_if<integral_type>(&other.data_);
      case json_class_type::boolean:
        return *std::get_if<boolean_type>(&other.data_);
      default:
        return null_type{};
    }
  }

private:
  data_type data_{null_type{}};
  json_class_type type_{json_class_type::null};
};

}  // namespace impl
}  // namespace json

#endif  // JSOLAR_TYPES_JSON_H_
