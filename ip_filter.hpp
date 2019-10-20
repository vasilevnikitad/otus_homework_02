#ifndef __IP_FILTER_H__
#define __IP_FILTER_H__
#include <algorithm>
#include <array>
#include <string>

namespace ip_filter {

  constexpr bool is_be_host_order()
  {
    return static_cast<uint8_t>(uint16_t{0x1}) == 0x00;
  }

  constexpr bool is_le_host_order()
  {
    return !is_be_host_order();
  }

  class ip {
    private:
      std::array<uint8_t, 4> address;
      friend std::ostream& operator << (std::ostream &, ip const &);

    public:
      ip() noexcept(true);

      explicit ip(const std::string &);

      template<typename... Args,
               typename = std::enable_if_t<std::conjunction_v<std::is_integral<Args>...> &&
                 sizeof...(Args) <= 4> >
      constexpr ip(Args&&... args) : address{std::forward<uint8_t>(args)...}
      {
        /*FIXME: I wanted a constexpr ctor but if the host machine is a little endian then
         * there is a problem with input args ordering. I didn't want to make too much
         * template code for it so i just reversing array in case of LE order.*/
        if( is_le_host_order() )
          std::reverse(std::begin(address), std::end(address));
      }

      constexpr uint8_t const &operator[](std::size_t idx) const
      { return address[idx]; }

      constexpr uint8_t &operator[](std::size_t idx)
      { return address[idx]; }

      constexpr operator uint32_t() const
      {
        return (address[0] << 0 ) | (address[1] << 8 ) |
               (address[2] << 16) | (address[3] << 24);
      }

  };

  class filter {
    private:
      ip ip_addr;
      ip ip_mask;
    public:
      template<typename... Args>
      constexpr filter(Args&&... args)
      : ip_addr{std::forward<uint8_t>(args)...},
        ip_mask{0xFF | std::forward<uint8_t>(args)...}
      { }

      /* FIXME: rename */
      inline bool is_valid(ip const &ip_addr_in) const
      {
        /*TODO: remove brackets if not needed */
        return (ip_addr & ip_mask) == (ip_addr_in & ip_mask);
      }

      template<typename Container>
      inline auto get_filtered_ip(Container &&in)
      {
        std::remove_reference_t<decltype(in)> out;

        std::copy_if(std::begin(in),
                     std::end(in),
                     std::back_insert_iterator<decltype(out)>(out),
                     [&](ip const &ip_addr){ return is_valid(ip_addr); });

        return out;
      }


  };

  class filter_any {
    protected:
      uint8_t filter_octet{0};

    public:
      constexpr filter_any(uint8_t const value) : filter_octet{value}
      { }

      /* FIXME: rename */
      inline bool is_valid(ip const &ip_addr) const
      {
        return ip_addr[0] == filter_octet ||
               ip_addr[1] == filter_octet ||
               ip_addr[2] == filter_octet ||
               ip_addr[3] == filter_octet;
      }

      template<typename Container>
      inline auto get_filtered_ip(Container &&in)
      {
        std::remove_reference_t<decltype(in)> out;

        std::copy_if(std::begin(in),
                     std::end(in),
                     std::back_insert_iterator<decltype(out)>(out),
                     [&](ip const &ip_addr){ return is_valid(ip_addr); });

        return out;
      }
  };

}

namespace std {
  string to_string(ip_filter::ip const &);
}
#endif
