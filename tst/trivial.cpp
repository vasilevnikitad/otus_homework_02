#define BOOST_TEST_MODULE trivial_module

#include "ip_filter.hpp"

#include <boost/test/unit_test.hpp>

#include <fstream>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

BOOST_AUTO_TEST_SUITE(trivial)

BOOST_AUTO_TEST_CASE(trivial_data) try {
  std::ifstream is{"trivial_data_input.txt"};

  if (!is)
    throw std::exception{"Input file doesn't exist"};

  std::stringstream ss;
  ip_filter::filter_pipe(
      is,
      ss,
      [](ip_filter::ip const &)  {return true;},
      [](ip_filter::ip const &ip){return ip[0] == 1;},
      [](ip_filter::ip const &ip){return ip[0] == 46 && ip[1] == 70;},
      [](ip_filter::ip const &ip){return std::any_of(std::begin(ip),
                                                     std::end(ip),
                                                     [](ip_filter::octet_t v){return v == 46;});}
      );
} catch(std::ios_base::failure const& e) {
  BOOST_FAIL(e.what());
} catch(std::exception const &e) {
  BOOST_FAIL(e.what());
} catch(...) {
  BOOST_FAIL("Unknown exception");
}

BOOST_AUTO_TEST_SUITE_END()
