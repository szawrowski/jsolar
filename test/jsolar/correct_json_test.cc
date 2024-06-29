#include <gtest/gtest.h>

#include "jsolar/jsolar.h"

TEST(JsolarTest, CorrectJsonMacro) {
  const auto config = json(
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
        "555-5678" ],
      "skills": [
        "C++",
        "Python",
        "JSON"
      ],
      "education": {
        "highschool": "Anytown High School",
        "university": {
          "name": "State University",
          "graduation_year" : 2020
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
  ASSERT_FALSE(config.has_error());
}

TEST(JsolarTest, CorrectJsonMacroDocument) {
  auto config = json::make_document();

  config["name"] = "John Doe";
  config["age"] = 30;

  config["array"] = json::make_array();
  config["object"] = json::make_object();

  config["is_student"] = false;

  config["address"] = json::make_object();
  config["address"]["street"] = "123 Main St";
  config["address"]["city"] = "Anytown";
  config["address"]["zip"] = "12345";

  config["phone_numbers"] = json::make_array();
  config["phone_numbers"].append("555-1234", "555-5678");

  config["skills"] = json::make_array("C++", "Python", "JSON");

  config["education"] = json::make_object();
  config["education"]["highschool"] = "Anytown High School";
  config["education"]["university"] = json::make_object();
  config["education"]["university"]["name"] = "State University";
  config["education"]["university"]["graduation_year"] = 2020;

  config["projects"] = json::make_array();
  config["projects"].append(json::make_object(), json::make_object());

  config["projects"][0]["title"] = "Project One";
  config["projects"][0]["description"] = "Description of Project One";

  config.add_member(json_path("projects", json_index(1), "title"),
                    "Project Two");
  config.add_member(json_path("projects", json_index(1), "description"),
                    "Description of Project Two");

  ASSERT_FALSE(config.has_error());
}
