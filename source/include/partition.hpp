/** partition.hpp
 * Short description here. 
 * 
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef PARTITION_HPP
#define PARTITION_HPP

#include <iterator>
#include <functional>
#include <list>

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
  class subrange
  {
  public:
    
    using iter   = typename Container::iterator;
    using citer  = typename Container::const_iterator;
    using riter  = std::reverse_iterator<iter>;
    using criter = std::reverse_iterator<citer>;
    using ref    = typename Container::reference;
    using cref   = typename Container::const_reference;
    
    subrange            (void)                = default;
    subrange            (const subrange&)     = default;
    subrange            (subrange&&) noexcept = default;
    subrange& operator= (const subrange&)     = default;
    subrange& operator= (subrange&&) noexcept = default;
    ~subrange           (void)                = default;
    
    template <typename Functor1, typename Functor2>
    constexpr subrange (Functor1&& begin_func, Functor2&& end_func)
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
    
    void swap (subrange other)
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
  
  template <typename Container, std::size_t N>
  class partition
  {
  public:
    
    using subrange_type = subrange<Container>;
    
    using iter   = typename std::array<subrange_type, N>::iterator;
    using citer  = typename std::array<subrange_type, N>::const_iterator;
    using riter  = typename std::array<subrange_type, N>::reverse_iterator;
    using criter = typename std::array<subrange_type, N>::const_reverse_iterator;
    using ref    = typename std::array<subrange_type, N>::reference;
    using cref   = typename std::array<subrange_type, N>::const_reference;
    
    partition            (void)                 = default;
    partition            (const partition&)     = default;
    partition            (partition&&) noexcept = default;
    partition& operator= (const partition&)     = default;
    partition& operator= (partition&&) noexcept = default;
    ~partition           (void)                 = default;
    
    template <typename ...EdgeFuncs,
              typename = typename std::enable_if<(N > 0 && (sizeof... (EdgeFuncs) == N + 1))>::type>
    explicit partition (EdgeFuncs&&... edges)
      : partition (std::integral_constant<std::size_t, 0> {}, std::forward<EdgeFuncs> (edges)...)
    { }
  
    template <typename Subrange, typename ...Subranges,
              typename = typename std::enable_if<(sizeof... (Subranges) == N) &&
                std::conjunction<std::is_same<subrange_type, Subrange>,
                                 std::is_same<subrange_type, Subranges>...>::value>::type>
    explicit partition (Subrange&& rng, Subranges&&... rngs)
      : m_subranges (std::forward<Subrange> (rng), std::forward<Subranges> (rngs)...)
    { }
  
    template <typename ...EdgeFuncs,
              typename = typename std::enable_if<(sizeof... (EdgeFuncs) == N - 1)>::type>
    explicit partition (Container& c, EdgeFuncs&&... edges)
      : partition (std::integral_constant<std::size_t, 0> {},
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

    GCH_CPP20_CONSTEXPR void swap (partition& other)
      noexcept (noexcept (adl::resolved::swap (std::declval<std::array<subrange_type, N>&> (),
                                               std::declval<std::array<subrange_type, N>&> ())))
    {
      using std::swap;
      swap (m_subranges, other.m_subranges);
    }

#if __cpp_lib_three_way_comparison >= 201907L
  
    GCH_NODISCARD
    friend constexpr bool operator== (const partition& lhs, const partition& rhs)
    {
      return lhs.m_subranges == rhs.m_subranges;
    }
  
    GCH_NODISCARD
    friend constexpr auto operator<=> (const partition& lhs, const partition& rhs)
    {
      return lhs.m_subranges <=> rhs.m_subranges;
    }

#else
    
    GCH_NODISCARD
    friend bool operator== (const partition& lhs, const partition& rhs)
    {
      return lhs.m_subranges == rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator!= (const partition& lhs, const partition& rhs)
    {
      return lhs.m_subranges != rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator< (const partition& lhs, const partition& rhs)
    {
      return lhs.m_subranges < rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator<= (const partition& lhs, const partition& rhs)
    {
      return lhs.m_subranges <= rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator> (const partition& lhs, const partition& rhs)
    {
      return lhs.m_subranges > rhs.m_subranges;
    }
    
    GCH_NODISCARD
    friend bool operator>= (const partition& lhs, const partition& rhs)
    {
      return lhs.m_subranges >= rhs.m_subranges;
    }
    
#endif
  
    template <std::size_t Index>
    friend GCH_CPP14_CONSTEXPR subrange<Container>& get (partition& p) noexcept
    {
      return std::get<Index> (p.m_subranges);
    }
  
    template <std::size_t Index>
    friend GCH_CPP14_CONSTEXPR subrange<Container>&& get (partition&& p) noexcept
    {
      return std::get<Index> (p.m_subranges);
    }
  
    template <std::size_t Index>
    friend GCH_CPP14_CONSTEXPR const subrange<Container>& get (const partition& p) noexcept
    {
      return std::get<Index> (p.m_subranges);
    }
  
    template <std::size_t Index>
    friend GCH_CPP14_CONSTEXPR const subrange<Container>&& get (const partition&& p) noexcept
    {
      return std::get<Index> (p.m_subranges);
    }
    
  private:
  
    template <typename EdgeFuncL, typename EdgeFuncR, typename ...EdgeFuncs, std::size_t Index>
    partition (std::integral_constant<std::size_t, Index>,
               EdgeFuncL&& edge_l, EdgeFuncR&& edge_r, EdgeFuncs&&... edges)
      : partition (std::integral_constant<std::size_t, Index + 1> { },
                   edge_r, std::forward<EdgeFuncs> (edges)...)
    {
      std::get<Index> (m_subranges) = { std::forward<EdgeFuncL> (edge_l),
                                        std::forward<EdgeFuncR> (edge_r) };
    }
  
    template <typename EdgeFuncL, typename EdgeFuncR>
    partition (std::integral_constant<std::size_t, N - 1>,
               EdgeFuncL&& edge_l, EdgeFuncR&& edge_end)
    {
      std::get<N - 1> (m_subranges) = { std::forward<EdgeFuncL> (edge_l),
                                        std::forward<EdgeFuncR> (edge_end) };
    }
    
    std::array<subrange_type, N> m_subranges;
  };

#if __cpp_lib_three_way_comparison >= 201907L
  
  namespace detail::compare
  {
    template <typename T>
    concept boolean_testable_impl = std::convertible_to<T, bool>;
    
    template<typename T>
    concept boolean_testable = boolean_testable_impl<T> &&
      requires (T&& t)
      {
        { !std::forward<T> (t) } -> boolean_testable_impl;
      };
    
    static constexpr struct synth_three_way_functor
    {
      template <typename T, typename U>
      constexpr auto operator() (const T& lhs, const U& rhs) noexcept (noexcept (lhs <=> rhs))
        requires std::three_way_comparable_with<T, U> &&
          requires
          {
            { lhs < rhs } -> boolean_testable;
            { lhs < rhs } -> boolean_testable;
          }
      {
        return lhs <=> rhs;
      }
      
      template <typename T, typename U>
      constexpr auto operator() (const T& lhs, const U& rhs)
        requires (! std::three_way_comparable_with<T, U>)
      {
        return (lhs < rhs) ? std::weak_ordering::less
                           : (rhs < lhs) ? std::weak_ordering::greater
                                         : std::weak_ordering::equivalent;
      }
    } synth_three_way;
  }
  
  template <typename Container>
  constexpr bool operator== (const subrange<Container>& lhs, const subrange<Container>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container>
  constexpr auto operator<=> (const subrange<Container>& lhs, const subrange<Container>& rhs)
  {
    return std::lexicographical_compare_three_way(lhs.begin (), lhs.end (),
                                                  rhs.begin (), rhs.end (),
                                                  detail::compare::synth_three_way);
  }
  
#else
  
  template <typename Container>
  bool operator== (const subrange<Container>& lhs, const subrange<Container>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container>
  bool operator!= (const subrange<Container>& lhs, const subrange<Container>& rhs)
  {
    return ! (lhs == rhs);
  }
  
  template <typename Container>
  bool operator< (const subrange<Container>& lhs, const subrange<Container>& rhs)
  {
    return std::lexicographical_compare (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container>
  bool operator<= (const subrange<Container>& lhs, const subrange<Container>& rhs)
  {
    return ! (lhs > rhs);
  }
  
  template <typename Container>
  bool operator> (const subrange<Container>& lhs, const subrange<Container>& rhs)
  {
    return std::lexicographical_compare (rhs.begin (), rhs.end (), lhs.begin (), lhs.end ());
  }
  
  template <typename Container>
  bool operator>= (const subrange<Container>& lhs, const subrange<Container>& rhs)
  {
    return ! (lhs < rhs);
  }
  
#endif
  
  template <typename Container>
  void swap (subrange<Container>& lhs, subrange<Container>& rhs)
    noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }
  
  template <std::size_t Index, typename Container, std::size_t N>
  GCH_CPP14_CONSTEXPR subrange<Container>& get (partition<Container, N>& p) noexcept;
  
  template <std::size_t Index, typename Container, std::size_t N>
  GCH_CPP14_CONSTEXPR subrange<Container>&& get (partition<Container, N>&& p) noexcept;
  
  template <std::size_t Index, typename Container, std::size_t N>
  GCH_CPP14_CONSTEXPR const subrange<Container>& get (const partition<Container, N>& p) noexcept;
  
  template <std::size_t Index, typename Container, std::size_t N>
  GCH_CPP14_CONSTEXPR const subrange<Container>&& get (const partition<Container, N>&& p) noexcept;
  
  template <typename Container, std::size_t N>
  void swap (partition<Container, N>& lhs, partition<Container, N>& rhs)
    noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }
}

namespace std
{
  template <typename T>
  struct tuple_size;
  
  template <typename Container, std::size_t N>
  struct tuple_size<gch::partition<Container, N>>
    : std::integral_constant<std::size_t, N>
  { };
  
  template <std::size_t I, typename T>
  struct tuple_element;
  
  template <std::size_t I, typename Container, std::size_t N>
  struct tuple_element<I, gch::partition<Container, N>>
  {
    using type = Container;
  };
}

#undef GCH_CPP14_CONSTEXPR
#undef GCH_CPP17_CONSTEXPR
#undef GCH_CPP20_CONSTEXPR
#undef GCH_NODISCARD

#endif // PARTITION_HPP
