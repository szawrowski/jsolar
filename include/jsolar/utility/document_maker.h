// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_UTIL_DOCUMENT_MAKER_H_
#define JSOLAR_UTIL_DOCUMENT_MAKER_H_

#include "jsolar/types/document.h"

namespace jsolar {

static Document MakeDocument() { return Document{}; }

static Document MakeDocument(const std::string& value) {
  return Document{value};
}

static Document MakeDocument(const Document& value) { return Document{value}; }

static Document MakeDocument(Document&& value) {
  return Document{std::move(value)};
}

static Document MakeDocument(const std::ifstream& stream) {
  return Document{stream};
}

}  // namespace jsolar

#endif  // JSOLAR_UTIL_DOCUMENT_MAKER_H_
