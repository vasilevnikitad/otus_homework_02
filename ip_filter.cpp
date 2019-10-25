#include "ip_filter.hpp"

#include <algorithm>
#include <array>
#include <exception>
#include <limits>
#include <string>

namespace ip_filter {


  ip::ip(const std::string &ip_str)
  {
    int pos{0};
    std::array<unsigned, IPV4_OCTET_CNT> val_le;
    if (std::sscanf(ip_str.c_str(), "%u.%u.%u.%u%n",
          &val_le[3], &val_le[2], &val_le[1], &val_le[0], &pos) != 4 ||
        (unsigned)pos != ip_str.length() ||
        std::any_of(std::begin(val_le),
                    std::end(val_le),
                    [](unsigned const val){return val > std::numeric_limits<octet_t>::max();})) {
      throw std::invalid_argument{"Argument doesn't contain valid ipv4 string"};
    }

    if constexpr (is_le_host_order())
      std::move(std::begin(val_le), std::end(val_le), std::begin(address));
    else
      std::move_backward(std::begin(val_le), std::end(val_le), std::end(address));
  }

  std::ostream& operator << (std::ostream &out, ip const &ip)
  {
    return out << std::to_string(ip);
  }
}

namespace std {
  string to_string(ip_filter::ip const &ip)
  {
    return std::to_string(ip[0]) + '.' + std::to_string(ip[1]) + '.' +
           std::to_string(ip[2]) + '.' + std::to_string(ip[3]);

  }
}
