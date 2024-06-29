#include <gtest/gtest.h>

#include "jsolar/jsolar.h"

TEST(JsolarTest, UnexpectedCharsFront) {
  const auto config = json(
    undexpected_chars
    {
      "name": "John Doe",
      "age": 30
    }
  );
  ASSERT_TRUE(config.has_error());
}

TEST(JsolarTest, UnexpectedCharsBack) {
  const auto config = json(
    {
      "name": "John Doe",
      "age": 30
    }
    undexpected_chars
  );
  ASSERT_TRUE(config.has_error());
}
