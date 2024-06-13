#include <fstream>

#include <gtest/gtest.h>

#include "jsolar/jsolar.h"

TEST(JsolarTest, IoOperator) {
  const auto output_config = jsolar(
    {
      "name": {
        "first": "John",
        "last": "Doe"
      },
      "age": 30,
      "address": {
        "street": "123 Main St",
        "city": "Anytown",
        "zip": "12345"
      },
      "phone_numbers": [
        "555-1234",
        "555-5678"
      ]
    }
  );
  ASSERT_FALSE(output_config.HasError());

  std::ofstream output_file{"operator_test.json"};
  if (output_file.is_open()) {
    output_file << output_config;
    output_file.close();
  }

  auto input_config = jsolar::MakeDocument();

  std::ifstream input_file{"operator_test.json"};
  if (input_file.is_open()) {
    input_file >> input_config;
    output_file.close();
  }

  ASSERT_FALSE(input_config.HasError());

  ASSERT_STREQ(output_config.ToString().c_str(),
               input_config.ToString().c_str());
}