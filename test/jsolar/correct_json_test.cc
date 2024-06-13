#include <gtest/gtest.h>

#include "jsolar/jsolar.h"

TEST(JsolarTest, CorrectJsonMacro) {
  const auto config = jsolar(
    {
      "name": "John Doe",
      "age": 30,
      "array": [],
      "object": {},
      "is_student": false,
      "address": {
        "street": "123 Main St",
        "city": "Anytown",
        "zip": "12345"
      },
      "phone_numbers": [
        "555-1234",
        "555-5678"
      ],
      "skills": [
        "C++",
        "Python",
        "JSON"
      ],
      "education": {
        "highschool": "Anytown High School",
        "university": {
          "name": "State University",
          "graduation_year": 2020
        }
      },
      "projects": [
        {
          "title": "Project One",
          "description": "Description of Project One"
        },
        {
          "title": "Project Two",
          "description": "Description of Project Two"
        }
      ]
    }
  );
  ASSERT_FALSE(config.HasError());
}

TEST(JsolarTest, CorrectJsonMacroDocument) {
  auto config = jsolar::MakeDocument();

  config["name"] = "John Doe";
  config["age"] = 30;

  config["array"] = jsolar::MakeArray();
  config["object"] = jsolar::MakeObject();

  config["is_student"] = false;

  config["address"] = jsolar::MakeObject();
  config["address"]["street"] = "123 Main St";
  config["address"]["city"] = "Anytown";
  config["address"]["zip"] = "12345";

  config["phone_numbers"] = jsolar::MakeArray();
  config["phone_numbers"].Append("555-1234", "555-5678");

  config["skills"] = jsolar::MakeArray("C++", "Python", "JSON");

  config["education"] = jsolar::MakeObject();
  config["education"]["highschool"] = "Anytown High School";
  config["education"]["university"] = jsolar::MakeObject();
  config["education"]["university"]["name"] = "State University";
  config["education"]["university"]["graduation_year"] = 2020;

  config["projects"] = jsolar::MakeArray();
  config["projects"].Append(jsolar::MakeObject(), jsolar::MakeObject());

  config["projects"][0]["title"] = "Project One";
  config["projects"][0]["description"] = "Description of Project One";

  config.AddMember(jsolarpath("projects", jsolaridx(1), "title"), "Project Two");
  config.AddMember(jsolarpath("projects", jsolaridx(1), "description"),
                   "Description of Project Two");

  ASSERT_FALSE(config.HasError());
}
