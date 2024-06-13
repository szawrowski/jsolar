# jsolar

### Using brackets operator
```c++
#include <iostream>
#include <jsolar/jsolar.h>

int main() {
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
  
  std::cout << config << std::endl;
  return 0;
}
```

### Using methods
```c++
#include <iostream>
#include <jsolar/jsolar.h>

int main() {
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
  
  std::cout << config << std::endl;
  return 0;
}
```

### Using native JSON
```c++
#include <iostream>
#include <jsolar/jsolar.h>

int main() {
  const auto config = jsolar(
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
config.ToString();
```
```json
{"name":{"first":"John","last":"Doe"},"age":30,"address":{"street":"123 Main St","city":"Anytown","zip":"12345"},"phone_numbers":["555-1234","555-5678"]}
```
#### Beauty
```c++
config.ToString(true);  // for mangling
config.ToString(true, 2);  // for mangling with indent specified (2 by default)
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
