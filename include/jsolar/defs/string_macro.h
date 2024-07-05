// Copyright (c) 2024, Alexander Szawrowski
//
// This file is distributed under the MIT License.
// See LICENSE file for details.

#ifndef JSOLAR_DEFS_STRING_MACRO_H_
#define JSOLAR_DEFS_STRING_MACRO_H_

#define json(...) cait::json::make_document(#__VA_ARGS__)

#define json_path(...) std::vector<std::string>{__VA_ARGS__}

#define json_index(index) std::to_string(index)

#endif  // JSOLAR_DEFS_STRING_MACRO_H_
