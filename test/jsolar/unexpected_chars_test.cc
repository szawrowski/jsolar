#include <gtest/gtest.h>

#include "jsolar/jsolar.h"

TEST(JsolarTest, UnexpectedCharsFront) {
  const auto config = jsolar(
    undexpected_chars
    {
      "name": "John Doe",
      "age": 30
    }
  );
  ASSERT_TRUE(config.HasError());
}

TEST(JsolarTest, UnexpectedCharsBack) {
  const auto config = jsolar(
    {
      "name": "John Doe",
      "age": 30
    }
    undexpected_chars
  );
  ASSERT_TRUE(config.HasError());
}
