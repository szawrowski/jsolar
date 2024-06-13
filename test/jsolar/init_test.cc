#include <gtest/gtest.h>

#include "jsolar/jsolar.h"

TEST(JsolarTest, InitBracketOperator) {
  auto config = jsolar::MakeDocument();
  config["name"] = jsolar::MakeObject();
  config["name"]["first"] = "John";
  config["name"]["last"] = "Doe";
  config["age"] = 30;
  config["address"] = jsolar::MakeObject();
  config["address"]["street"] = "123 Main St";
  config["address"]["city"] = "Anytown";
  config["address"]["zip"] = "12345";
  config["phone_numbers"] = jsolar::MakeArray("555-1234", "555-5678");
  ASSERT_FALSE(config.HasError());
}

TEST(JsolarTest, InitMethods) {
  auto config = jsolar::MakeDocument();
  config.AddMember("name", jsolar::MakeObject());
  config.AddMember(jsolarpath("name", "first"), "John");
  config.AddMember(jsolarpath("name", "second"), "Doe");
  config.AddMember("age", 30);
  config.AddMember("address", jsolar::MakeObject());
  config.AddMember(jsolarpath("address", "street"), "123 Main St");
  config.AddMember(jsolarpath("address", "city"), "Anytown");
  config.AddMember(jsolarpath("address", "zip"), "12345");
  config.AddMember("phone_numbers", jsolar::MakeArray("555-1234", "555-5678"));

  ASSERT_FALSE(config.HasError());
}

TEST(JsolarTest, InitNative) {
  const auto config = jsolar(
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
  ASSERT_FALSE(config.HasError());
}
