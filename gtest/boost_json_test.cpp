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
}