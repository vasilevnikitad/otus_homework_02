#include "ip_filter.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

template<typename C>
void apply_filters(C&& c){};

template<typename C, typename Arg, typename ...Args>
void apply_filters(C&& c, Arg&& arg, Args&&... args) {

  for(auto const& ip: ip_filter::filter{std::forward<Arg>(arg)}(std::forward<C>(c)))
    std::cout << ip << '\n';

  apply_filters(std::forward<C>(c), std::forward<Args>(args)...);
}

int main(int, char const *[]) try
{
  std::vector<ip_filter::ip> ip_pool;

  for(std::string line; std::getline(std::cin, line); )
    ip_pool.emplace_back(line.substr(0, line.find('\t')));

  std::sort(std::begin(ip_pool), std::end(ip_pool), std::greater<ip_filter::ip>());

  apply_filters(ip_pool,
      [](ip_filter::ip const &ip){return true;},
      [](ip_filter::ip const &ip){return ip[0] == 1;},
      [](ip_filter::ip const &ip){return ip[0] == 46 && ip[1] == 70;},
      [](ip_filter::ip const &ip){return ip[0] == 46 || ip[1] == 46 ||
                                         ip[2] == 46 || ip[3] == 46;}
      );

  return EXIT_SUCCESS;
} catch(std::exception const &e) {
    std::cerr << "Exception: "<< e.what() << std::endl;
    return EXIT_FAILURE;
}
