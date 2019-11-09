#include "ip_filter.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <ostream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace ip_filter {

  ip_base ip::str2ip_base(const std::string &ip_str) {
    int pos{0};
    std::array<unsigned, IPV4_OCTET_CNT> val_be{{0, 0, 0, 0}};
    if (std::sscanf(ip_str.c_str(), "%u.%u.%u.%u%n",
          &val_be[0], &val_be[1], &val_be[2], &val_be[3], &pos) != 4 ||
        (unsigned)pos != ip_str.length() ||
        std::any_of(std::begin(val_be),
          std::end(val_be),
          [](unsigned const val){return val > std::numeric_limits<octet_t>::max();})) {
      throw std::invalid_argument{"Argument doesn't contain valid ipv4 string"};
    }
    return {
      static_cast<octet_t>(val_be[0]),
      static_cast<octet_t>(val_be[1]),
      static_cast<octet_t>(val_be[2]),
      static_cast<octet_t>(val_be[3])
    };
  }

  template <typename InputIt, typename OutputIt, typename ...Args>
  void apply_filters(InputIt begin, InputIt end, OutputIt out, Args&&... args)
  {
    (std::copy_if(begin, end, out, std::forward<Args>(args)), ...);
  }

  template <typename IS, typename OS, typename ...Filters>
  void filter_pipe(IS &&is, OS &&os, Filters&&... filters)
  {
    std::vector<ip> ip_pool;

    for(std::string line; std::getline(std::forward<IS>(is), line); )
      ip_pool.emplace_back(line.substr(0, line.find('\t')));

    std::sort(std::begin(ip_pool), std::end(ip_pool), std::greater<ip>());

    apply_filters(std::begin(ip_pool), std::end(ip_pool),
                  std::ostream_iterator<ip>(std::forward<OS>(os), "\n"),
                  filters...);
  }

}
