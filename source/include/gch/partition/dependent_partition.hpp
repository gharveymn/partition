/** dependent_partition.hpp
 * Short description here. 
 * 
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef PARTITION_DEPENDENT_PARTITION_HPP
#define PARTITION_DEPENDENT_PARTITION_HPP

#include <array>
#include <functional>

#ifndef GCH_CPP14_CONSTEXPR
#  if __cpp_constexpr >= 201304L
#    define GCH_CPP14_CONSTEXPR constexpr
#  else
#    define GCH_CPP14_CONSTEXPR
#  endif
#endif

#ifndef GCH_CPP17_CONSTEXPR
#  if __cpp_constexpr >= 201603L
#    define GCH_CPP17_CONSTEXPR constexpr
#  else
#    define GCH_CPP17_CONSTEXPR
#  endif
#endif

#ifndef GCH_CPP20_CONSTEXPR
#  if __cpp_constexpr >= 201907L
#    define GCH_CPP20_CONSTEXPR constexpr
#  else
#    define GCH_CPP20_CONSTEXPR
#  endif
#endif

#ifndef GCH_NODISCARD
#  if __has_cpp_attribute(nodiscard) >= 201603L
#    define GCH_NODISCARD [[nodiscard]]
#  else
#    define GCH_NODISCARD
#  endif
#endif

namespace gch
{
  template <typename Container>
  class dependent_subrange;
  
  template <typename Container, std::size_t N,
            typename SubrangeType = dependent_subrange<Container>>
  class dependent_partition_view;
  
  namespace adl
  {
    using std::distance;
    using std::swap;
    namespace resolved
    {
      template <typename It>
      auto distance (It lhs, It rhs) noexcept (noexcept (distance (lhs, rhs)))
      -> decltype (distance (lhs, rhs));
      
      template <typename It>
      auto swap (It& lhs, It& rhs) noexcept (noexcept (swap (lhs, rhs)))
      -> decltype (swap (lhs, rhs));
    }
  }
  
  template <typename Container>
  class dependent_subrange
  {
  public:
    
    using iter   = typename Container::iterator;
    using citer  = typename Container::const_iterator;
    using riter  = std::reverse_iterator<iter>;
    using criter = std::reverse_iterator<citer>;
    using ref    = typename Container::reference;
    using cref   = typename Container::const_reference;
    
    dependent_subrange (void)                                     = default;
    dependent_subrange (const dependent_subrange&)                = default;
    dependent_subrange (dependent_subrange&&) noexcept            = default;
    dependent_subrange& operator= (const dependent_subrange&)     = default;
    dependent_subrange& operator= (dependent_subrange&&) noexcept = default;
    ~dependent_subrange (void)                                    = default;
    
    template <typename Functor1, typename Functor2>
    constexpr dependent_subrange (Functor1&& begin_func, Functor2&& end_func)
    noexcept
      : m_begin_func (std::forward<Functor1> (begin_func)),
        m_end_func (std::forward<Functor2> (end_func))
    { }
    
    GCH_NODISCARD iter  begin    (void)       noexcept { return m_begin_func ();         }
    GCH_NODISCARD citer begin    (void) const noexcept { return citer (m_begin_func ()); }
    GCH_NODISCARD citer cbegin   (void) const noexcept { return citer (m_begin_func ()); }
    
    GCH_NODISCARD iter  end      (void)       noexcept { return m_end_func (); }
    GCH_NODISCARD citer end      (void) const noexcept { return citer (m_end_func ());   }
    GCH_NODISCARD citer cend     (void) const noexcept { return citer (m_end_func ());   }
    
    GCH_NODISCARD riter  rbegin  (void)       noexcept { return riter (end ());     }
    GCH_NODISCARD criter rbegin  (void) const noexcept { return criter (end ());    }
    GCH_NODISCARD criter crbegin (void) const noexcept { return criter (cend ());   }
    
    GCH_NODISCARD riter  rend    (void)       noexcept { return riter (begin ());   }
    GCH_NODISCARD criter rend    (void) const noexcept { return criter (begin ());  }
    GCH_NODISCARD criter crend   (void) const noexcept { return criter (cbegin ()); }
    
    GCH_NODISCARD ref&  front    (void)       noexcept { return *begin ();          }
    GCH_NODISCARD cref& front    (void) const noexcept { return *cbegin ();         }
    
    GCH_NODISCARD ref&  back     (void)       noexcept { return *(--end ());        }
    GCH_NODISCARD cref& back     (void) const noexcept { return *(--cend ());       }
    
    GCH_NODISCARD auto size (void) const
    noexcept (noexcept (adl::resolved::distance (std::declval<iter> (), std::declval<iter> ())))
    -> decltype (adl::resolved::distance (std::declval<iter> (), std::declval<iter> ()))
    {
      using std::distance;
      return distance (begin (), end ());
    }
    
    GCH_NODISCARD bool empty (void) const noexcept { return begin () == end (); }
    
    void swap (dependent_subrange other)
    noexcept (noexcept (adl::resolved::swap (std::declval<std::function<iter ()>&> (),
                                             std::declval<std::function<iter ()>&> ())))
    {
      using std::swap;
      swap (m_begin_func, other.m_begin_func);
      swap (m_end_func, other.m_end_func);
    }
  
  private:
    
    std::function<iter ()> m_begin_func;
    std::function<iter ()> m_end_func;
  };
  
  template <typename Container, std::size_t N, typename SubrangeType>
  class dependent_partition_view
  {
    template<typename ...>
    struct conjunction : std::true_type
    { };
    
    template<class T>
    struct conjunction<T> : T
    { };
    
    template<class T, class ...Ts>
    struct conjunction<T, Ts...> : std::conditional<bool (T::value), conjunction<Ts...>, T>::type
    { };
  
  public:
    using subrange_type = SubrangeType;
    
    using iter   = typename std::array<subrange_type, N>::iterator;
    using citer  = typename std::array<subrange_type, N>::const_iterator;
    using riter  = typename std::array<subrange_type, N>::reverse_iterator;
    using criter = typename std::array<subrange_type, N>::const_reverse_iterator;
    using ref    = typename std::array<subrange_type, N>::reference;
    using cref   = typename std::array<subrange_type, N>::const_reference;
    
    dependent_partition_view (void)                                           = default;
    dependent_partition_view (const dependent_partition_view&)                = default;
    dependent_partition_view (dependent_partition_view&&) noexcept            = default;
    dependent_partition_view& operator= (const dependent_partition_view&)     = default;
    dependent_partition_view& operator= (dependent_partition_view&&) noexcept = default;
    ~dependent_partition_view (void)                                          = default;
    
    template <typename ...EdgeFuncs,
              typename = typename std::enable_if<(N > 0 && (sizeof... (EdgeFuncs) == N + 1))>::type>
    constexpr explicit dependent_partition_view (EdgeFuncs&&... edges)
      : dependent_partition_view (std::integral_constant<std::size_t, 0> {},
                                  std::forward<EdgeFuncs> (edges)...)
    { }
    
    template <typename Subrange, typename ...Subranges,
              typename = typename std::enable_if<(sizeof... (Subranges) == N) &&
                                                   conjunction<std::is_same<subrange_type, Subrange>,
                                                 std::is_same<subrange_type, Subranges>...>::value>::type>
    constexpr explicit dependent_partition_view (Subrange&& rng, Subranges&&... rngs)
      : m_subranges (std::forward<Subrange> (rng), std::forward<Subranges> (rngs)...)
    { }
    
    template <typename ...EdgeFuncs,
              typename = typename std::enable_if<(sizeof... (EdgeFuncs) == N - 1)>::type>
    constexpr explicit dependent_partition_view (Container& c, EdgeFuncs&&... edges)
      : dependent_partition_view (std::integral_constant<std::size_t, 0> {},
                                  [&c] (void) -> typename subrange_type::iter { return c.begin (); },
                                  std::forward<EdgeFuncs> (edges)...,
                                  [&c] (void) -> typename subrange_type::iter { return c.end (); })
    { }
    
    GCH_NODISCARD iter    begin   (void)       noexcept { return m_subranges.begin ();   }
    GCH_NODISCARD citer   begin   (void) const noexcept { return m_subranges.begin ();   }
    GCH_NODISCARD citer   cbegin  (void) const noexcept { return m_subranges.cbegin ();  }
    
    GCH_NODISCARD iter    end     (void)       noexcept { return m_subranges.end ();     }
    GCH_NODISCARD citer   end     (void) const noexcept { return m_subranges.end ();     }
    GCH_NODISCARD citer   cend    (void) const noexcept { return m_subranges.cend ();    }
    
    GCH_NODISCARD riter   rbegin  (void)       noexcept { return m_subranges.rbegin ();  }
    GCH_NODISCARD criter  rbegin  (void) const noexcept { return m_subranges.rbegin ();  }
    GCH_NODISCARD criter  crbegin (void) const noexcept { return m_subranges.crbegin (); }
    
    GCH_NODISCARD riter   rend    (void)       noexcept { return m_subranges.rend ();    }
    GCH_NODISCARD criter  rend    (void) const noexcept { return m_subranges.rend ();    }
    GCH_NODISCARD criter  crend   (void) const noexcept { return m_subranges.crend ();   }
    
    GCH_NODISCARD ref&    front   (void)       noexcept { return m_subranges.front ();   }
    GCH_NODISCARD cref&   front   (void) const noexcept { return m_subranges.front ();   }
    
    GCH_NODISCARD ref&    back    (void)       noexcept { return m_subranges.back ();    }
    GCH_NODISCARD cref&   back    (void) const noexcept { return m_subranges.back ();    }
    
    GCH_NODISCARD
      GCH_CPP17_CONSTEXPR subrange_type& at (std::size_t pos) noexcept (false)
    {
      return m_subranges.at (pos);
    }
    
    GCH_NODISCARD
      GCH_CPP14_CONSTEXPR const subrange_type& at (std::size_t pos) const noexcept (false)
    {
      return m_subranges.at (pos);
    }
    
    GCH_NODISCARD
      GCH_CPP17_CONSTEXPR subrange_type& operator[] (std::size_t pos) noexcept
    {
      return m_subranges.operator[] (pos);
    }
    
    GCH_NODISCARD
      GCH_CPP14_CONSTEXPR const subrange_type& operator[] (std::size_t pos) const noexcept
    {
      return m_subranges.operator[] (pos);
    }
    
    GCH_NODISCARD
      GCH_CPP14_CONSTEXPR subrange_type * data (void) noexcept
    {
      return m_subranges.data ();
    }
    
    GCH_NODISCARD
    constexpr const subrange_type * data (void) const noexcept
    {
      return m_subranges.data ();
    }
    
    GCH_NODISCARD static constexpr std::size_t size (void) noexcept { return N; }
    GCH_NODISCARD static constexpr bool empty (void) noexcept { return N == 0; }
    
    GCH_NODISCARD constexpr std::size_t max_size (void) const noexcept
    {
      return m_subranges.max_size ();
    }
    
    GCH_CPP20_CONSTEXPR void fill (const subrange_type& s)
    {
      m_subranges.fill (s);
    }
    
    GCH_CPP20_CONSTEXPR void swap (dependent_partition_view& other)
    noexcept (noexcept (adl::resolved::swap (std::declval<std::array<subrange_type, N>&> (),
                                             std::declval<std::array<subrange_type, N>&> ())))
    {
      using std::swap;
      swap (m_subranges, other.m_subranges);
    }

#if __cpp_lib_three_way_comparison >= 201907L
    
    GCH_NODISCARD
    friend constexpr bool operator== (const dependent_partition_view& lhs, const dependent_partition_view& rhs)
    {
      return lhs.m_subranges == rhs.m_subranges;
    }
  
    GCH_NODISCARD
    friend constexpr auto operator<=> (const dependent_partition_view& lhs, const dependent_partition_view& rhs)
    {
      return lhs.m_subranges <=> rhs.m_subranges;
    }

#else
    
    GCH_NODISCARD
    friend bool operator== (const dependent_partition_view& lhs, const dependent_partition_view& rhs)
    {
      return lhs.m_subranges == rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator!= (const dependent_partition_view& lhs, const dependent_partition_view& rhs)
    {
      return lhs.m_subranges != rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator< (const dependent_partition_view& lhs, const dependent_partition_view& rhs)
    {
      return lhs.m_subranges < rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator<= (const dependent_partition_view& lhs, const dependent_partition_view& rhs)
    {
      return lhs.m_subranges <= rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator> (const dependent_partition_view& lhs, const dependent_partition_view& rhs)
    {
      return lhs.m_subranges > rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator>= (const dependent_partition_view& lhs, const dependent_partition_view& rhs)
    {
      return lhs.m_subranges >= rhs.m_subranges;
    }

#endif
    
    template <std::size_t Index>
    GCH_CPP14_CONSTEXPR subrange_type& get (void)& noexcept
    {
      return std::get<Index> (m_subranges);
    }
    
    template <std::size_t Index>
    GCH_CPP14_CONSTEXPR subrange_type&& get (void)&& noexcept
    {
      return std::get<Index> (m_subranges);
    }
    
    template <std::size_t Index>
    constexpr const subrange_type& get (void) const& noexcept
    {
      return std::get<Index> (m_subranges);
    }
    
    template <std::size_t Index>
    constexpr const subrange_type&& get (void) const&& noexcept
    {
      return std::get<Index> (m_subranges);
    }
  
  private:
    
    template <typename EdgeFuncL, typename EdgeFuncR, typename ...EdgeFuncs, std::size_t Index>
    GCH_CPP14_CONSTEXPR dependent_partition_view (std::integral_constant<std::size_t, Index>,
                                                  EdgeFuncL&& edge_l, EdgeFuncR&& edge_r,
                                                  EdgeFuncs&&... edges)
      : dependent_partition_view (std::integral_constant<std::size_t, Index + 1> { },
                                  edge_r, std::forward<EdgeFuncs> (edges)...)
    {
      std::get<Index> (m_subranges) = { std::forward<EdgeFuncL> (edge_l),
                                        std::forward<EdgeFuncR> (edge_r) };
    }
    
    template <typename EdgeFuncL, typename EdgeFuncR>
    GCH_CPP14_CONSTEXPR dependent_partition_view (std::integral_constant<std::size_t, N - 1>,
                                                  EdgeFuncL&& edge_l, EdgeFuncR&& edge_end)
    {
      std::get<N - 1> (m_subranges) = { std::forward<EdgeFuncL> (edge_l),
                                        std::forward<EdgeFuncR> (edge_end) };
    }
    
    std::array<subrange_type, N> m_subranges;
  };

#if __cpp_lib_three_way_comparison >= 201907L
  
  template <typename Container>
  constexpr bool operator== (const dependent_subrange<Container>& lhs,
                             const dependent_subrange<Container>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container>
  constexpr auto operator<=> (const dependent_subrange<Container>& lhs,
                              const dependent_subrange<Container>& rhs)
  {
    return std::lexicographical_compare_three_way (lhs.begin (), lhs.end (),
                                                   rhs.begin (), rhs.end (),
                                                   detail::compare::synth_three_way);
  }

#else
  
  template <typename Container>
  bool operator== (const dependent_subrange<Container>& lhs, const dependent_subrange<Container>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container>
  bool operator!= (const dependent_subrange<Container>& lhs, const dependent_subrange<Container>& rhs)
  {
    return ! (lhs == rhs);
  }
  
  template <typename Container>
  bool operator< (const dependent_subrange<Container>& lhs, const dependent_subrange<Container>& rhs)
  {
    return std::lexicographical_compare (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container>
  bool operator<= (const dependent_subrange<Container>& lhs, const dependent_subrange<Container>& rhs)
  {
    return ! (lhs > rhs);
  }
  
  template <typename Container>
  bool operator> (const dependent_subrange<Container>& lhs, const dependent_subrange<Container>& rhs)
  {
    return std::lexicographical_compare (rhs.begin (), rhs.end (), lhs.begin (), lhs.end ());
  }
  
  template <typename Container>
  bool operator>= (const dependent_subrange<Container>& lhs, const dependent_subrange<Container>& rhs)
  {
    return ! (lhs < rhs);
  }

#endif
  
  template <typename Container>
  void swap (dependent_subrange<Container>& lhs, dependent_subrange<Container>& rhs)
  noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }
  
  template <std::size_t Index, typename Container, std::size_t N>
  constexpr dependent_subrange<Container>& get (dependent_partition_view<Container, N>& p) noexcept
  {
    return p.template get<Index> ();
  }
  
  template <std::size_t Index, typename Container, std::size_t N>
  constexpr dependent_subrange<Container>&& get (dependent_partition_view<Container, N>&& p) noexcept
  {
    return p.template get<Index> ();
  }
  
  template <std::size_t Index, typename Container, std::size_t N>
  constexpr const dependent_subrange<Container>& get (const dependent_partition_view<Container, N>& p) noexcept
  {
    return p.template get<Index> ();
  }
  
  template <std::size_t Index, typename Container, std::size_t N>
  constexpr const dependent_subrange<Container>&& get (const dependent_partition_view<Container, N>&& p) noexcept
  {
    return p.template get<Index> ();
  }
  
  template <typename Container, std::size_t N>
  void swap (dependent_partition_view<Container, N>& lhs, dependent_partition_view<Container, N>& rhs)
  noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }
}

namespace std
{
  template <typename Container, std::size_t N>
  struct tuple_size<gch::dependent_partition_view<Container, N>>
    : public std::integral_constant<std::size_t, N>
  { };
  
  template <std::size_t I, typename Container, std::size_t N>
  struct tuple_element<I, gch::dependent_partition_view<Container, N>>
  {
    using type = typename gch::dependent_partition_view<Container, N>::subrange_type;
  };
}

#endif // PARTITION_DEPENDENT_PARTITION_HPP