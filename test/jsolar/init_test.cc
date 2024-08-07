#include <gtest/gtest.h>

#include "jsolar/jsolar.h"

TEST(JsolarTest, InitBracketOperator) {
  auto config = cait::json::make_document();
  config["name"] = cait::json::make_object();
  config["name"]["first"] = "John";
  config["name"]["last"] = "Doe";
  config["age"] = 30;
  config["address"] = cait::json::make_object();
  config["address"]["street"] = "123 Main St";
  config["address"]["city"] = "Anytown";
  config["address"]["zip"] = "12345";
  config["phone_numbers"] = cait::json::make_array("555-1234", "555-5678");
  ASSERT_FALSE(config.has_error());
}

TEST(JsolarTest, InitMethods) {
  auto config = cait::json::make_document();
  config.add_member("name", cait::json::make_object());
  config.add_member(json_path("name", "first"), "John");
  config.add_member(json_path("name", "second"), "Doe");
  config.add_member("age", 30);
  config.add_member("address", cait::json::make_object());
  config.add_member(json_path("address", "street"), "123 Main St");
  config.add_member(json_path("address", "city"), "Anytown");
  config.add_member(json_path("address", "zip"), "12345");
  config.add_member("phone_numbers",
                    cait::json::make_array("555-1234", "555-5678"));

  ASSERT_FALSE(config.has_error());
}

TEST(JsolarTest, InitNative) {
  const auto config = json(
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
  ASSERT_FALSE(config.has_error());
}
