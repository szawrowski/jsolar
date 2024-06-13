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

namespace jsolar {
namespace impl {

class Json {
public:
  using ArrayType = std::deque<Json*>;
  using ObjectType = std::vector<std::pair<std::string, Json*>>;
  using StringType = std::string;
  using NullType = std::nullptr_t;
  using FloatingType = double;
  using IntegralType = long long;
  using BooleanType = bool;

  using DataType = std::variant<NullType, ObjectType, ArrayType, StringType,
                                FloatingType, IntegralType, BooleanType>;

public:
  Json() = default;

  Json(const Json& other) : data_{CopyData(other)}, type_{other.type_} {}

  Json(Json&& other) noexcept
      : data_{std::move(other.data_)}, type_{other.type_} {
    other.data_ = NullType{};
    other.type_ = JsonClassType::kNull;
  }

  template <typename T>
  Json(T value, std::enable_if_t<std::is_same_v<T, BooleanType>>* = nullptr)
      : data_{value}, type_{JsonClassType::kBoolean} {}

  template <typename T>
  Json(T value, std::enable_if_t<std::is_integral_v<T> &&
                                 !std::is_same_v<T, BooleanType>>* = nullptr)
      : data_{static_cast<IntegralType>(value)},
        type_{JsonClassType::kIntegral} {}

  template <typename T>
  Json(T value, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr)
      : data_{static_cast<FloatingType>(value)},
        type_{JsonClassType::kFloating} {}

  template <typename T>
  Json(T value,
       std::enable_if_t<std::is_convertible_v<T, StringType>>* = nullptr)
      : data_{std::string{value}}, type_{JsonClassType::kString} {}

  ~Json() { ClearInternal(); }

public:
  Json& operator=(const Json& other) {
    if (this != &other) {
      ClearInternal();
      data_ = CopyData(other);
      type_ = other.type_;
    }
    return *this;
  }

  Json& operator=(Json&& other) noexcept {
    if (this != &other) {
      ClearInternal();
      data_ = std::move(other.data_);
      type_ = other.type_;
      other.data_ = NullType{};
      other.type_ = JsonClassType::kNull;
    }
    return *this;
  }

  template <typename T>
  std::enable_if_t<std::is_same_v<T, BooleanType>, Json&> operator=(T value) {
    SetType(JsonClassType::kBoolean);
    data_ = value;
    return *this;
  }

  template <typename T>
  std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, BooleanType>,
                   Json&>
  operator=(T value) {
    SetType(JsonClassType::kIntegral);
    data_ = static_cast<IntegralType>(value);
    return *this;
  }

  template <typename T>
  std::enable_if_t<std::is_floating_point_v<T>, Json&> operator=(T value) {
    SetType(JsonClassType::kFloating);
    data_ = static_cast<FloatingType>(value);
    return *this;
  }

  template <typename T>
  std::enable_if_t<std::is_convertible_v<T, StringType>, Json&> operator=(
      T value) {
    SetType(JsonClassType::kString);
    data_ = std::string{value};
    return *this;
  }

public:
  Json& operator[](const std::string& key) {
    SetType(JsonClassType::kObject);
    const auto object = std::get_if<ObjectType>(&data_);
    const auto it =
        std::find_if(object->begin(), object->end(),
                     [&key](const auto& pair) { return pair.first == key; });
    if (it == object->end()) {
      object->emplace_back(key, new Json{});
      return *object->back().second;
    }
    return *it->second;
  }

  Json& operator[](const size_t index) {
    SetType(JsonClassType::kArray);
    const auto array = std::get_if<ArrayType>(&data_);
    if (index >= array->size()) {
      array->resize(index + 1, nullptr);
    }
    if (!array->at(index)) {
      new (array->at(index)) Json;
    }
    return *array->at(index);
  }

public:
  Json& At(const std::string& key) { return operator[](key); }

  [[nodiscard]] const Json& At(const std::string& key) const {
    const auto object = std::get_if<ObjectType>(&data_);
    const auto it =
        std::find_if(object->begin(), object->end(),
                     [&key](const auto& pair) { return pair.first == key; });
    if (it == object->end()) {
      throw std::out_of_range{"Key not found"};
    }
    return *it->second;
  }

  Json& At(const size_t index) { return operator[](index); }

  [[nodiscard]] const Json& At(const size_t index) const {
    return *std::get_if<ArrayType>(&data_)->at(index);
  }

public:
  template <typename T>
  void Append(T arg) {
    if (!IsArray()) {
      SetType(JsonClassType::kArray);
    }
    std::get_if<ArrayType>(&data_)->emplace_back(new Json{arg});
  }

  template <typename T, typename... U>
  void Append(T arg, U... args) {
    Append(arg);
    Append(args...);
  }

public:
  [[nodiscard]] size_t Size() const {
    if (IsArray()) {
      return std::get_if<ArrayType>(&data_)->size();
    }
    if (IsObject()) {
      return std::get_if<ObjectType>(&data_)->size();
    }
    return std::numeric_limits<size_t>::max();
  }

  [[nodiscard]] size_t Length() const {
    if (IsArray()) {
      return std::get_if<ArrayType>(&data_)->size();
    }
    return std::numeric_limits<size_t>::max();
  }

public:
  [[nodiscard]] bool HasMember(const std::string& key) const {
    if (IsObject()) {
      const auto& object = std::get_if<ObjectType>(&data_);
      return std::find_if(object->begin(), object->end(),
                          [&key](const auto& pair) {
                            return pair.first == key;
                          }) != object->end();
    }
    return false;
  }

  [[nodiscard]] bool IsNull() const { return type_ == JsonClassType::kNull; }

  [[nodiscard]] bool IsObject() const {
    return type_ == JsonClassType::kObject;
  }

  [[nodiscard]] bool IsArray() const { return type_ == JsonClassType::kArray; }

  [[nodiscard]] bool IsString() const {
    return type_ == JsonClassType::kString;
  }

  [[nodiscard]] bool IsFloating() const {
    return type_ == JsonClassType::kFloating;
  }

  [[nodiscard]] bool IsIntegral() const {
    return type_ == JsonClassType::kIntegral;
  }

  [[nodiscard]] bool IsBoolean() const {
    return type_ == JsonClassType::kBoolean;
  }

public:
  ObjectType& GetData() {
    if (type_ != JsonClassType::kObject) {
      throw std::logic_error{"Trying to access non-object types as object"};
    }
    return *std::get_if<ObjectType>(&data_);
  }

  [[nodiscard]] JsonClassType GetType() const { return type_; }

  [[nodiscard]] StringType GetString() const {
    return IsString() ? EscapeString(*std::get_if<StringType>(&data_))
                      : StringType{};
  }

  [[nodiscard]] FloatingType GetFloating() const {
    return IsFloating() ? *std::get_if<FloatingType>(&data_) : FloatingType{};
  }

  [[nodiscard]] IntegralType GetIntegral() const {
    return IsIntegral() ? *std::get_if<IntegralType>(&data_) : IntegralType{};
  }

  [[nodiscard]] BooleanType GetBoolean() const {
    return IsBoolean() ? *std::get_if<BooleanType>(&data_) : BooleanType{};
  }

public:
  [[nodiscard]] std::string ToString(const bool mangling = false,
                                     const size_t indent = 2) const {
    std::ostringstream oss;
    ConstructOutput(oss, mangling, indent, indent);
    return oss.str();
  }

  static Json internal_make(const JsonClassType type) {
    Json obj;
    obj.SetType(type);
    return obj;
  }

private:
  void ConstructOutput(std::ostringstream& oss, const bool mangling,
                       const size_t base_indent, const size_t indent) const {
    const std::string indent_str(indent, GetSymbol(CharHex::Space));

    switch (type_) {
      case JsonClassType::kObject: {
        oss << GetSymbol(CharHex::OpeningCurlyBracket);
        const auto data = std::get_if<ObjectType>(&data_);
        if (data && !data->empty()) {
          if (mangling) {
            oss << GetSymbol(CharHex::LineFeed);
          }
          bool first = true;
          for (const auto& [key, value] : *data) {
            if (!first) {
              oss << GetSymbol(CharHex::Comma);
              if (mangling) {
                oss << GetSymbol(CharHex::LineFeed);
              }
            }
            if (mangling) {
              oss << indent_str;
            }
            oss << GetSymbol(CharHex::DoubleQuote) << EscapeString(key)
                << GetSymbol(CharHex::DoubleQuote) << GetSymbol(CharHex::Colon)
                << GetSymbol(CharHex::Space);
            value->ConstructOutput(oss, mangling, base_indent,
                                   indent + base_indent);
            first = false;
          }
          if (mangling) {
            oss << GetSymbol(CharHex::LineFeed)
                << std::string(indent - base_indent, GetSymbol(CharHex::Space));
          }
        }
        oss << GetSymbol(CharHex::ClosingCurlyBracket);
        break;
      }
      case JsonClassType::kArray: {
        oss << GetSymbol(CharHex::OpeningSquareBracket);
        const auto data = std::get_if<ArrayType>(&data_);
        if (data && !data->empty()) {
          if (mangling) {
            oss << GetSymbol(CharHex::LineFeed);
          }
          bool first = true;
          for (const auto& item : *data) {
            if (!first) {
              oss << GetSymbol(CharHex::Comma);
              if (mangling) {
                oss << GetSymbol(CharHex::LineFeed);
              }
            }
            if (mangling) {
              oss << indent_str;
            }
            item->ConstructOutput(oss, mangling, base_indent,
                                  indent + base_indent);
            first = false;
          }
          if (mangling) {
            oss << GetSymbol(CharHex::LineFeed)
                << std::string(indent - base_indent, GetSymbol(CharHex::Space));
          }
        }
        oss << GetSymbol(CharHex::ClosingSquareBracket);
        break;
      }
      case JsonClassType::kNull:
        oss << GetNullStr();
        break;
      case JsonClassType::kString:
        oss << GetSymbol(CharHex::DoubleQuote)
            << EscapeString(*std::get_if<StringType>(&data_))
            << GetSymbol(CharHex::DoubleQuote);
        break;
      case JsonClassType::kFloating:
        oss << *std::get_if<FloatingType>(&data_);
        break;
      case JsonClassType::kIntegral:
        oss << *std::get_if<IntegralType>(&data_);
        break;
      case JsonClassType::kBoolean:
        oss << (*std::get_if<BooleanType>(&data_) ? GetBooleanStr(true)
                                                  : GetBooleanStr(false));
        break;
    }
  }

  void SetType(const JsonClassType type) {
    if (type_ != type) {
      ClearInternal();
      type_ = type;
      switch (type_) {
        case JsonClassType::kNull:
          data_ = NullType{};
          break;
        case JsonClassType::kObject:
          data_ = ObjectType{};
          break;
        case JsonClassType::kArray:
          data_ = ArrayType{};
          break;
        case JsonClassType::kString:
          data_ = StringType{};
          break;
        case JsonClassType::kFloating:
          data_ = FloatingType{};
          break;
        case JsonClassType::kIntegral:
          data_ = IntegralType{};
          break;
        case JsonClassType::kBoolean:
          data_ = BooleanType{};
          break;
      }
    }
  }

  void ClearInternal() {
    switch (type_) {
      case JsonClassType::kObject: {
        auto& object = *std::get_if<ObjectType>(&data_);
        for (auto it = object.begin(); it != object.end(); ++it) {
          delete it->second;
        }
        object.clear();
        break;
      }
      case JsonClassType::kArray: {
        auto& array = *std::get_if<ArrayType>(&data_);
        for (auto* item : array) {
          delete item;
        }
        array.clear();
        break;
      }
      case JsonClassType::kString:
        std::get_if<StringType>(&data_)->clear();
        break;
      default:
        type_ = JsonClassType::kNull;
    }
  }

  static DataType CopyData(const Json& other) {
    switch (other.type_) {
      case JsonClassType::kObject: {
        auto object = ObjectType{};
        const auto other_object = std::get_if<ObjectType>(&other.data_);
        for (const auto& [key, value] : *other_object) {
          object.emplace_back(key, new Json{*value});
        }
        return object;
      }
      case JsonClassType::kArray: {
        auto array = ArrayType{};
        const auto other_array = std::get_if<ArrayType>(&other.data_);
        for (const auto& item : *other_array) {
          array.emplace_back(new Json{*item});
        }
        return array;
      }
      case JsonClassType::kNull:
        return *std::get_if<NullType>(&other.data_);
      case JsonClassType::kString:
        return *std::get_if<StringType>(&other.data_);
      case JsonClassType::kFloating:
        return *std::get_if<FloatingType>(&other.data_);
      case JsonClassType::kIntegral:
        return *std::get_if<IntegralType>(&other.data_);
      case JsonClassType::kBoolean:
        return *std::get_if<BooleanType>(&other.data_);
      default:
        return NullType{};
    }
  }

private:
  DataType data_{NullType{}};
  JsonClassType type_{JsonClassType::kNull};
};

}  // namespace impl
}  // namespace jsolar

#endif  // JSOLAR_TYPES_JSON_H_
