#include <algorithm>
#include <array>
#include <cassert>
#include <ostream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using octet_t = std::uint8_t;
enum : std::uint8_t { IPV4_OCTET_CNT = 4 };

class ip : public std::array<octet_t, IPV4_OCTET_CNT> {
  private:

    inline friend std::ostream& operator << (std::ostream &out, ip const &ip)
    {
      bool dot{false};

      for (auto const &octet: ip)
        out << (dot ? "." : (dot = true, "")) << +octet;

      return out;
    }

    static std::array<octet_t, IPV4_OCTET_CNT> get(const std::string &ip_str) {
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

  public:
    template<typename... Args,
      typename = std::enable_if_t<std::conjunction_v<std::is_integral<Args>...> &&
        sizeof...(Args) <= IPV4_OCTET_CNT> >
        constexpr ip(Args&&... args) : std::array<octet_t, IPV4_OCTET_CNT>{std::forward<octet_t>(args)...} { }

    ip(const std::string &ip_str) : std::array<octet_t, IPV4_OCTET_CNT>{get(ip_str)} { }
};

template <typename InputIt, typename OutputIt, typename ...Args,
  typename = std::enable_if_t<std::conjunction_v<std::is_invocable_r<bool, Args, ip const &>...>>>
void apply_filters(InputIt begin, InputIt end, OutputIt out, Args&&... args)
{
  (std::copy_if(begin, end, out, std::forward<Args>(args)), ...);
}

int main(int, char const *[]) try
{
  std::vector<ip> ip_pool;

  for(std::string line; std::getline(std::cin, line); )
    ip_pool.emplace_back(line.substr(0, line.find('\t')));

  std::sort(std::begin(ip_pool), std::end(ip_pool), std::greater<ip>());

  apply_filters(std::begin(ip_pool), std::end(ip_pool),
                std::ostream_iterator<ip>(std::cout, "\n"),
                [](ip const &)  {return true;},
                [](ip const &ip){return ip[0] == 1;},
                [](ip const &ip){return ip[0] == 46 && ip[1] == 70;},
                [](ip const &ip){return std::any_of(std::begin(ip),
                                                    std::end(ip),
                                                    [](octet_t v){ return v == 46;});}
  );

  return EXIT_SUCCESS;
} catch(std::exception const &e) {
    std::cerr << "Exception: "<< e.what() << std::endl;
    return EXIT_FAILURE;
}
