// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_DEFS_CHAR_HEX_H_
#define JSOLAR_DEFS_CHAR_HEX_H_

namespace jsolar {
namespace impl {

enum class CharHex : unsigned char {
  AudibleBell = 0x07,
  Beckspace = 0x08,
  HorizontalTab = 0x09,
  LineFeed = 0x0A,
  VerticalTab = 0x0B,
  FormFeed = 0x0C,
  CarriageReturn = 0x0D,
  Space = 0x20,
  DoubleQuote = 0x22,
  SingleQuote = 0x27,
  Plus = 0x2B,
  Comma = 0x2C,
  Minus = 0x2D,
  Point = 0x2E,
  Slash = 0x2F,
  Colon = 0x3A,
  Semicolon = 0x3B,
  QuestionMark = 0x3F,
  CapitalE = 0x45,
  OpeningSquareBracket = 0x5B,
  Backslash = 0x5C,
  ClosingSquareBracket = 0x5D,
  LowercaseA = 0x61,
  LowercaseB = 0x62,
  LowercaseE = 0x65,
  LowercaseF = 0x66,
  LowercaseN = 0x6E,
  LowercaseR = 0x72,
  LowercaseT = 0x74,
  LowercaseU = 0x75,
  LowercaseV = 0x76,
  OpeningCurlyBracket = 0x7B,
  ClosingCurlyBracket = 0x7D,
};

static constexpr auto GetSymbol(const CharHex value) {
  return static_cast<unsigned char>(value);
}

}  // namespace impl
}  // namespace jsolar

#endif  // JSOLAR_DEFS_CHAR_HEX_H_
