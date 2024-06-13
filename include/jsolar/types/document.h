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

namespace jsolar {

class Document {
public:
  using JsonType = impl::Json;
  using JsonParserType = impl::JsonParser;

public:
  Document() = default;

  Document(const std::string& value) { Parse(value); }

  Document(const std::ifstream& stream) { Parse(stream); }

  Document(const Document& other) : root_{other.root_}, error_{other.error_} {}

  Document(Document&& other) noexcept
      : root_{std::move(other.root_)}, error_{std::move(other.error_)} {
    other.root_ = MakeNull();
    other.error_ = std::make_pair(JsonParseErrorType::kNoError,
                                  std::numeric_limits<size_t>::max());
  }

  ~Document() = default;

public:
  Document& operator=(const std::string& value) {
    Parse(value);
    return *this;
  }

  Document& operator=(const std::ifstream& stream) {
    Parse(stream);
    return *this;
  }

  Document& operator=(const Document& other) {
    if (this != &other) {
      root_ = other.root_;
      error_ = other.error_;
    }
    return *this;
  }

  Document& operator=(Document&& other) noexcept {
    if (this != &other) {
      root_ = std::move(other.root_);
      error_ = other.error_;
      other.root_ = MakeNull();
      other.error_ = std::make_pair(JsonParseErrorType::kNoError, 0);
    }
    return *this;
  }

public:
  JsonType& operator[](const std::string& key) { return root_[key]; }

  const JsonType& operator[](const std::string& key) const {
    return root_.At(key);
  }

public:
  void Parse(const std::string& value) {
    if (const auto parser = JsonParserType::Parse(value);
        parser.HasError()) {
      error_ = parser.GetError();
      root_ = MakeNull();
    } else {
      error_ = std::make_pair(JsonParseErrorType::kNoError, 0);
      root_ = parser.GetData();
    }
  }

  void Parse(const std::ifstream& ifs) {
    std::ostringstream buffer;
    buffer << ifs.rdbuf();

    Parse(buffer.str());
  }

  void AddMember(const std::string& key, const JsonType& value) {
    root_[key] = value;
  }

  void AddMember(const std::vector<std::string>& keys, const JsonType& value) {
    JsonType* current = &root_;

    // Traverse through keys to navigate the nested structure
    for (const auto& key : keys) {
      if (current->IsArray() && impl::IsNumber(key)) {
        // Convert key to array index if current object is an array
        const size_t index = std::stoull(key);
        // Expand the array if index is greater than current size
        while (index >= current->Size()) {
          current->Append(JsonType{});
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

  void RemoveMember(const std::string& key) {
    if (root_.HasMember(key)) {
      auto& obj = root_.GetData();
      obj.erase(std::remove_if(
                    obj.begin(), obj.end(),
                    [&key](const auto& pair) { return pair.first == key; }),
                obj.end());
    }
  }

  [[nodiscard]] bool HasMember(const std::string& key) const {
    return root_.HasMember(key);
  }

  [[nodiscard]] size_t Size() const { return root_.Size(); }

  [[nodiscard]] bool HasError() const {
    return error_.first != JsonParseErrorType::kNoError;
  }

  [[nodiscard]] JsonParseErrorType GetError() const { return error_.first; }

  [[nodiscard]] auto GetErrorString() const {
    return impl::GetJsonParseErrorString(error_);
  }

  [[nodiscard]] JsonClassType GetType() const { return root_.GetType(); }

  [[nodiscard]] std::string ToString(const bool mangling = false,
                                     const size_t indent = 2) const {
    return root_.ToString(mangling, indent);
  }

private:
  JsonType root_{MakeObject()};
  std::pair<JsonParseErrorType, size_t> error_{JsonParseErrorType::kNoError, 0};
};

static std::istream& operator>>(std::istream& is, Document& value) {
  std::ostringstream buffer;
  buffer << is.rdbuf();
  value.Parse(buffer.str());
  return is;
}

static std::ostream& operator<<(std::ostream& os, const Document& value) {
  os << value.ToString();
  return os;
}

}  // namespace jsolar

#endif  // JSOLAR_TYPES_DOCUMENT_H_
