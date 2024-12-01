#include <gtest/gtest.h>  // Add this line

#include <boost/json.hpp>
#include <cstddef>  // For std::ptrdiff_t
#include <limits>   // For std::numeric_limits

using namespace boost;

TEST(BoostExampleTest, JsonValue) {
  json::value jv;
  ASSERT_TRUE(jv.is_null());
  // checked
  ASSERT_ANY_THROW(jv.as_bool());

  auto& o = jv.get_object();
  ASSERT_TRUE(jv.is_null());
  bool b = jv.get_bool();
  ASSERT_FALSE(b);
  ASSERT_TRUE(jv.try_as_string().has_error());
  std::cout << jv << std::endl;

  jv.emplace_string() = "Hello, world!";
  ASSERT_TRUE(jv.is_string());
  jv.emplace_array() = {1, 2, 3};
  ASSERT_TRUE(jv.is_array());

  json::value jv1 = {{"a", 1}};
  ASSERT_TRUE(jv1.get_object().if_contains("c") == nullptr);
  ASSERT_FALSE(jv1.as_object().contains("x"));

  json::value ajv = jv1.as_object()["x"];
  std::cout << "the kind: " << ajv.kind() << " ." << std::endl;
  ASSERT_TRUE(ajv.is_null());
  jv1.as_object()["x"] = {1, 2, 3};
  ASSERT_TRUE(jv1.at("x").is_array());
  jv1.as_object()["y"].emplace_array() = 5;  // array of size 5.
  std::cout << "y kind: " << jv1.at("y").kind() << " ." << std::endl;
  std::cout << "y value: " << jv1.at("y") << " ." << std::endl;
  ASSERT_TRUE(jv1.at("y").is_array());
  ASSERT_EQ(jv1.at("y").kind(), json::kind::array);
  jv1.at("y").at(0) = 1;
  ASSERT_EQ(jv1.at("y").at(0), 1);
  jv1.as_object()["y"].as_array().emplace_back(7);
  ASSERT_EQ(jv1.at("y").as_array().size(), 6);

  // will always create new type.
  jv1.as_object()["y"].emplace_string() = "hello";
  ASSERT_TRUE(jv1.at("y").is_string());
  ASSERT_EQ(jv1.at("y").as_string().size(), 5);

  json::value jv10;
  ASSERT_ANY_THROW(jv10.as_object()["x"] = 0);

  // unchecked. will cause SIGSEGV
  // try {
  //   jv10.get_object()["x"] = 0;
  // } catch (...) {
  //   std::cerr << "Unchecked error." << std::endl;
  // }

  if (auto obj = jv10.if_object()) {
    std::cout << "jv10 is object." << std::endl;
  } else {
    jv10.emplace_object();
  }
  ASSERT_TRUE(jv10.is_object());
}