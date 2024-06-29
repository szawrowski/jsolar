// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_TYPES_DOCUMENT_H_
#define JSOLAR_TYPES_DOCUMENT_H_

#include <fstream>

#include "jsolar/defs/json_parse_error_string.h"
#include "jsolar/parser/json_parser.h"
#include "jsolar/utility/value_maker.h"

namespace json {

class document_t {
public:
  using json_type = impl::json_t;
  using json_parser_type = impl::json_parser_t;

public:
  document_t() = default;

  document_t(const std::string& value) { parse(value); }

  document_t(const std::ifstream& stream) { parse(stream); }

  document_t(const document_t& other)
      : root_{other.root_}, error_{other.error_} {}

  document_t(document_t&& other) noexcept
      : root_{std::move(other.root_)}, error_{std::move(other.error_)} {
    other.root_ = make_null();
    other.error_ = std::make_pair(json_parse_error_type::no_error,
                                  std::numeric_limits<size_t>::max());
  }

  ~document_t() = default;

public:
  document_t& operator=(const std::string& value) {
    parse(value);
    return *this;
  }

  document_t& operator=(const std::ifstream& stream) {
    parse(stream);
    return *this;
  }

  document_t& operator=(const document_t& other) {
    if (this != &other) {
      root_ = other.root_;
      error_ = other.error_;
    }
    return *this;
  }

  document_t& operator=(document_t&& other) noexcept {
    if (this != &other) {
      root_ = std::move(other.root_);
      error_ = other.error_;
      other.root_ = make_null();
      other.error_ = std::make_pair(json_parse_error_type::no_error, 0);
    }
    return *this;
  }

public:
  json_type& operator[](const std::string& key) { return root_[key]; }

  const json_type& operator[](const std::string& key) const {
    return root_.at(key);
  }

public:
  void parse(const std::string& value) {
    if (const auto parser = json_parser_type::parse(value);
        parser.has_error()) {
      error_ = parser.get_error();
      root_ = make_null();
    } else {
      error_ = std::make_pair(json_parse_error_type::no_error, 0);
      root_ = parser.get_data();
    }
  }

  void parse(const std::ifstream& ifs) {
    std::ostringstream buffer;
    buffer << ifs.rdbuf();

    parse(buffer.str());
  }

  void add_member(const std::string& key, const json_type& value) {
    root_[key] = value;
  }

  void add_member(const std::vector<std::string>& keys,
                  const json_type& value) {
    json_type* current = &root_;

    // Traverse through keys to navigate the nested structure
    for (const auto& key : keys) {
      if (current->is_array() && impl::is_number(key)) {
        // Convert key to array index if current object is an array
        const size_t index = std::stoull(key);
        // Expand the array if index is greater than current size
        while (index >= current->size()) {
          current->append(json_type{});
        }
        // Move to the new array element
        current = &(*current)[index];
      } else {
        // Access by key if current object is an object
        current = &(*current)[key];
      }
    }
    // Assign the value at the end of navigation
    *current = value;
  }

  void remove_member(const std::string& key) {
    if (root_.has_member(key)) {
      auto& obj = root_.get_data();
      obj.erase(std::remove_if(
                    obj.begin(), obj.end(),
                    [&key](const auto& pair) { return pair.first == key; }),
                obj.end());
    }
  }

  [[nodiscard]] bool has_member(const std::string& key) const {
    return root_.has_member(key);
  }

  [[nodiscard]] size_t size() const { return root_.size(); }

  [[nodiscard]] bool has_error() const {
    return error_.first != json_parse_error_type::no_error;
  }

  [[nodiscard]] json_parse_error_type get_error() const { return error_.first; }

  [[nodiscard]] auto get_error_string() const {
    return impl::get_json_parse_error_string(error_);
  }

  [[nodiscard]] json_class_type get_type() const { return root_.get_type(); }

  [[nodiscard]] std::string to_string(const bool mangling = false,
                                      const size_t indent = 2) const {
    return root_.to_string(mangling, indent);
  }

private:
  json_type root_{make_object()};
  std::pair<json_parse_error_type, size_t> error_{
      json_parse_error_type::no_error, 0};
};

static std::istream& operator>>(std::istream& is, document_t& value) {
  std::ostringstream buffer;
  buffer << is.rdbuf();
  value.parse(buffer.str());
  return is;
}

static std::ostream& operator<<(std::ostream& os, const document_t& value) {
  os << value.to_string();
  return os;
}

}  // namespace json

#endif  // JSOLAR_TYPES_DOCUMENT_H_
