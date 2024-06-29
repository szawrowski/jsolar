# json

### Using brackets operator
```c++
#include <iostream>
#include <jsolar/jsolar.h>

int main() {
  auto config = json::make_document();
  config["name"] = json::make_object();
  config["name"]["first"] = "John";
  config["name"]["last"] = "Doe";
  config["age"] = 30;
  config["address"] = json::make_object();
  config["address"]["street"] = "123 Main St";
  config["address"]["city"] = "Anytown";
  config["address"]["zip"] = "12345";
  config["phone_numbers"] = json::make_array("555-1234", "555-5678");
  
  std::cout << config << std::endl;
  return 0;
}
```

### Using methods
```c++
#include <iostream>
#include <jsolar/jsolar.h>

int main() {
  auto config = json::make_document();
  config.add_member("name", json::make_object());
  config.add_member(json_path("name", "first"), "John");
  config.add_member(json_path("name", "second"), "Doe");
  config.add_member("age", 30);
  config.add_member("address", json::make_object());
  config.add_member(json_path("address", "street"), "123 Main St");
  config.add_member(json_path("address", "city"), "Anytown");
  config.add_member(json_path("address", "zip"), "12345");
  config.add_member("phone_numbers", json::make_array("555-1234", "555-5678"));
  
  std::cout << config << std::endl;
  return 0;
}
```

### Using native JSON
```c++
#include <iostream>
#include <jsolar/jsolar.h>

int main() {
  const auto config = json(
    {
      "name": {
        "first": "John",
        "last": "Doe"
      },
      "age": 30
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
  std::cout << config << std::endl;
  return 0;
}
```

### Output strings
#### Optimized
```c++
config.to_string();
```
```json
{"name":{"first":"John","last":"Doe"},"age":30,"address":{"street":"123 Main St","city":"Anytown","zip":"12345"},"phone_numbers":["555-1234","555-5678"]}
```
#### Beauty
```c++
config.to_string(true);  // for mangling
config.to_string(true, 2);  // for mangling with indent specified (2 by default)
```
```json
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
```
