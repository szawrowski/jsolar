// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_UTIL_DOCUMENT_MAKER_H_
#define JSOLAR_UTIL_DOCUMENT_MAKER_H_

#include "jsolar/types/document.h"

namespace cait {
namespace json {

static document_t make_document() { return document_t{}; }

static document_t make_document(const std::string& value) {
  return document_t{value};
}

static document_t make_document(const document_t& value) {
  return document_t{value};
}

static document_t make_document(document_t&& value) {
  return document_t{std::move(value)};
}

static document_t make_document(const std::ifstream& stream) {
  return document_t{stream};
}

}  // namespace json
}  // namespace cait

#endif  // JSOLAR_UTIL_DOCUMENT_MAKER_H_
