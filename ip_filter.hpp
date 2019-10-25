#ifndef __IP_FILTER_H__
#define __IP_FILTER_H__
#include <algorithm>
#include <array>
#include <string>

namespace ip_filter {

  constexpr bool is_be_host_order()
  {
    return static_cast<std::uint8_t>(uint16_t{0x1}) == 0x00;
  }

  constexpr bool is_le_host_order()
  {
    return !is_be_host_order();
  }

  class ip {
    private:
      using octet_t = std::uint8_t;
      static constexpr uint8_t IPV4_OCTET_CNT{4};

      std::array<octet_t, IPV4_OCTET_CNT> address;
      friend std::ostream& operator << (std::ostream &, ip const &);

    public:

      ip() noexcept(true);

      explicit ip(const std::string &);

      template<typename... Args,
               typename = std::enable_if_t<std::conjunction_v<std::is_integral<Args>...> &&
                 sizeof...(Args) <= IPV4_OCTET_CNT> >
      constexpr ip(Args&&... args) : address{std::forward<octet_t>(args)...}
      {
        /*FIXME: I wanted a constexpr ctor but if the host machine is a little endian then
         * there is a problem with input args ordering. I didn't want to make too much
         * template code for it so i just reversing array in case of LE order.*/
        if( is_le_host_order() )
          std::reverse(std::begin(address), std::end(address));
      }

      constexpr octet_t const &operator[](std::size_t idx) const
      {
        if( is_le_host_order() )
          idx = (IPV4_OCTET_CNT - 1) - idx;

        return address[idx];
      }

      constexpr octet_t &operator[](std::size_t idx)
      {
        if( is_le_host_order() )
          idx = (IPV4_OCTET_CNT - 1) - idx;

        return address[idx];
      }

      constexpr operator uint32_t() const
      {
        return (address[0] << 0 ) | (address[1] << 8 ) |
               (address[2] << 16) | (address[3] << 24);
      }

  };

  template<typename Filter_fn,
           typename = std::enable_if_t<std::is_invocable_r<bool,
                                                           Filter_fn,
                                                           ip const&>::value>>
  class filter {
    private:
      Filter_fn is_passed;

    public:
      /*FIXME: How to use template in this situation? */
      filter(Filter_fn &&fun) : is_passed{fun} { };
      filter(Filter_fn &fun) : is_passed{fun} { };

      template<typename Container>
      inline auto operator() (Container &&in)
      {
        using ContainerType = std::decay_t<Container>;

        ContainerType out;

        std::copy_if(std::begin(in),
                     std::end(in),
                     std::back_insert_iterator<ContainerType>(out),
                     is_passed);
        return out;
      }
  };

}

namespace std {
  string to_string(ip_filter::ip const &);
}
#endif
