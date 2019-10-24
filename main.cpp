#include "ip_filter.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#if 0 /* Comlilers don't support it :/ */
#include <execution>
#endif
#include <iostream>
#include <string>
#include <vector>

int main(int, char const *[]) try
{
  std::vector<ip_filter::ip> ip_pool;

  for(std::string line; std::getline(std::cin, line); )
    ip_pool.emplace_back(line.substr(0, line.find('\t')));

  std::sort(std::begin(ip_pool),
            std::end(ip_pool),
            std::greater<ip_filter::ip>());

  for(auto const &ip: ip_pool)
    std::cout << ip << std::endl;

  for(auto const &ip: ip_filter::filter{1}.get_filtered_ip(ip_pool))
    std::cout << ip << std::endl;

  for(auto const &ip: ip_filter::filter{46, 70}.get_filtered_ip(ip_pool))
    std::cout << ip << std::endl;

  for(auto const &ip: ip_filter::filter_any{46}.get_filtered_ip(ip_pool))
    std::cout << ip << std::endl;

  return EXIT_SUCCESS;
} catch(std::exception const &e) {
    std::cerr << "Exception: "<< e.what() << std::endl;
    return EXIT_FAILURE;
}
