#include <gtest/gtest.h>  // Add this line

#include <boost/json.hpp>
#include <boost/url.hpp>
#include <cstddef>  // For std::ptrdiff_t
#include <limits>   // For std::numeric_limits

using namespace boost;

TEST(BoostExampleTest, stringtoken) {
  urls::url_view url(
      "http://www.example.com:8080/path/to/"
      "resource?query=string&query2=string2#fragment");

  std::string_view sv =
      url.path().substr(1);  // default return_string, it's temporary.
  std::string s;
  sv = url.path(urls::string_token::preserve_size(s))
           .substr(1);  // default return_string, it's temporary.
  ASSERT_EQ(sv, "path/to/resource");
  ASSERT_EQ(s, "/path/to/resource");
}