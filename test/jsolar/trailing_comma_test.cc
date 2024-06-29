#include <gtest/gtest.h>

#include "jsolar/jsolar.h"

TEST(JsolarTest, TrailingCommaInObject) {
  const auto config = json(
    {
      "key" : "value",
    }
  );
  ASSERT_TRUE(config.has_error());
}

TEST(JsolarTest, TrailingCommaInArray) {
  const auto config = json(
    {
      "key": [1, 2, 3, 4, 5,]
    }
  );
  ASSERT_TRUE(config.has_error());
}
