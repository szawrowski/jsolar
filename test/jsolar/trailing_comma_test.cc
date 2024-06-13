#include <gtest/gtest.h>

#include "jsolar/jsolar.h"

TEST(JsolarTest, TrailingCommaInObject) {
  const auto config = jsolar(
    {
      "key": "value",
    }
  );
  ASSERT_TRUE(config.HasError());
}

TEST(JsolarTest, TrailingCommaInArray) {
  const auto config = jsolar(
    {
      "key": [1, 2, 3, 4, 5,]
    }
  );
  ASSERT_TRUE(config.HasError());
}
