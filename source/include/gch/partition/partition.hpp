/** partition.hpp
 * Short description here. 
 * 
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GCH_PARTITION_PARTITION_HPP
#define GCH_PARTITION_PARTITION_HPP

#include <array>
#include <functional>

#if __cplusplus >= 201703L
#  if __has_include(<variant>)
#    include <variant>
#    if __cpp_lib_variant >= 201606L
#      ifndef GCH_HAS_VARIANT
#        define GCH_HAS_VARIANT
#      endif
#    endif
#  endif
#endif

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
  
  template <typename Iterator>
  class subrange_view
  {
  public:
    using iter      = Iterator;
    using iter_diff = typename std::iterator_traits<iter>::difference_type;
    using iter_val  = typename std::iterator_traits<iter>::value_type;
    using iter_ptr  = typename std::iterator_traits<iter>::value_type;
    using iter_ref  = typename std::iterator_traits<iter>::value_type;
    
    subrange_view            (void)                               = default;
    subrange_view            (const subrange_view&)     = default;
    subrange_view            (subrange_view&&) noexcept = default;
    subrange_view& operator= (const subrange_view&)     = default;
    subrange_view& operator= (subrange_view&&) noexcept = default;
    ~subrange_view           (void)                               = default;
    
    subrange_view (iter first, iter last)
      : m_first (first),
        m_last  (last)
    { }
    
    iter  begin (void) const noexcept { return m_first; }
    iter  end   (void) const noexcept { return m_last;  }
  
    iter_ref front (void) const noexcept { return *begin (); }
    iter_ref back (void) const noexcept { return *(--end ()); }
    
    std::size_t size (void) const noexcept { return std::distance (m_first, m_last); }
    bool empty (void) const noexcept { return m_first == m_last; }
    
    GCH_NODISCARD subrange_view next (iter_diff count = 1) const &
    {
      subrange_view ret = *this;
      ret.advance (count);
      return ret;
    }
    
    GCH_NODISCARD subrange_view next (iter_diff count = 1) &&
    {
      advance (count);
      return std::move (*this);
    }
    
    GCH_NODISCARD subrange_view prev (iter_diff count = 1) const
    {
      subrange_view ret = *this;
      ret.advance (-count);
      return ret;
    }
  
    subrange_view& advance (iter_diff count)
    {
      std::advance (m_first, count);
    }
  
  private:
    iter m_first;
    iter m_last;
  };
  
  template <typename Partition, std::size_t N>
  class partition_view
  {
  public:
    using partition_type     = Partition;
    using container_type     = typename partition_type::container_type;
    using subrange_view_type = decltype (std::declval<partition_type> ().template subrange_view<0> ());
    using view_array         = std::array<subrange_view_type, N>;
    using nonconst_partition = typename std::add_const<Partition>::type;
    
    partition_view            (void)                      = default;
    partition_view            (const partition_view&)     = default;
    partition_view            (partition_view&&) noexcept = default;
    partition_view& operator= (const partition_view&)     = default;
    partition_view& operator= (partition_view&&) noexcept = default;
    ~partition_view           (void)                      = default;
  
    explicit partition_view (partition_type& p)
    {
      init_views<0> (p);
    }
    
    // copy from view of `partition_type` to view of `const partition_type`
    template <typename NonConst,
      typename = typename std::enable_if<std::is_same<nonconst_partition, NonConst>::value>::type>
    /* implicit */ partition_view (const partition_view<NonConst, N>& other)
      : m_subrange_views (other.m_subrange_views)
    { }
    
    // move from view of `partition_type` to view of `const partition_type`
    template <typename NonConst,
              typename = typename std::enable_if<std::is_same<nonconst_partition, NonConst>::value>::type>
    /* implicit */ partition_view (partition_view<NonConst, N>&& other)
      : m_subrange_views (std::move (other.m_subrange_views))
    { }
    
    using iter    = typename view_array::iterator;
    using citer   = typename view_array::const_iterator;
    using riter   = typename view_array::reverse_iterator;
    using criter  = typename view_array::const_reverse_iterator;
    using ref     = typename view_array::reference;
    using cref    = typename view_array::const_reference;
    
    iter   begin   (void)       noexcept { return m_subrange_views.begin ();   }
    citer  begin   (void) const noexcept { return m_subrange_views.begin ();   }
    citer  cbegin  (void) const noexcept { return m_subrange_views.cbegin ();  }
    
    iter   end     (void)       noexcept { return m_subrange_views.end ();     }
    citer  end     (void) const noexcept { return m_subrange_views.end ();     }
    citer  cend    (void) const noexcept { return m_subrange_views.cend ();    }
    
    riter  rbegin  (void)       noexcept { return m_subrange_views.rbegin ();  }
    criter rbegin  (void) const noexcept { return m_subrange_views.rbegin ();  }
    criter crbegin (void) const noexcept { return m_subrange_views.crbegin (); }
    
    riter  rend    (void)       noexcept { return m_subrange_views.rend ();    }
    criter rend    (void) const noexcept { return m_subrange_views.rend ();    }
    criter crend   (void) const noexcept { return m_subrange_views.crend ();   }
    
    ref&   front   (void)       noexcept { return m_subrange_views.front ();   }
    cref&  front   (void) const noexcept { return m_subrange_views.front ();   }
    
    ref&   back    (void)       noexcept { return m_subrange_views.back ();    }
    cref&  back    (void) const noexcept { return m_subrange_views.back ();    }
  
    GCH_NODISCARD
    GCH_CPP17_CONSTEXPR subrange_view_type& at (std::size_t pos) noexcept (false)
    {
      return m_subrange_views.at (pos);
    }
  
    GCH_NODISCARD
    GCH_CPP14_CONSTEXPR const subrange_view_type& at (std::size_t pos) const noexcept (false)
    {
      return m_subrange_views.at (pos);
    }
  
    GCH_NODISCARD
    GCH_CPP17_CONSTEXPR subrange_view_type& operator[] (std::size_t pos) noexcept
    {
      return m_subrange_views.operator[] (pos);
    }
  
    GCH_NODISCARD
    GCH_CPP14_CONSTEXPR const subrange_view_type& operator[] (std::size_t pos) const noexcept
    {
      return m_subrange_views.operator[] (pos);
    }
  
    GCH_NODISCARD
    GCH_CPP14_CONSTEXPR subrange_view_type * data (void) noexcept
    {
      return m_subrange_views.data ();
    }
  
    GCH_NODISCARD
    constexpr const subrange_view_type * data (void) const noexcept
    {
      return m_subrange_views.data ();
    }
  
    GCH_NODISCARD static constexpr std::size_t size (void) noexcept { return N; }
    GCH_NODISCARD static constexpr bool empty (void) noexcept { return N == 0; }
  
    GCH_NODISCARD constexpr std::size_t max_size (void) const noexcept
    {
      return m_subrange_views.max_size ();
    }
  
    GCH_CPP20_CONSTEXPR void swap (partition_view& other)
      noexcept (noexcept (adl::resolved::swap (
        std::declval<std::array<subrange_view_type, N>&> (),
        std::declval<std::array<subrange_view_type, N>&> ())))
    {
      using std::swap;
      swap (m_subrange_views, other.m_subrange_views);
    }
  
    template <std::size_t Index>
    GCH_CPP14_CONSTEXPR subrange_view_type& get (void)& noexcept
    {
      return std::get<Index> (m_subrange_views);
    }
  
    template <std::size_t Index>
    GCH_CPP14_CONSTEXPR subrange_view_type&& get (void)&& noexcept
    {
      return std::get<Index> (m_subrange_views);
    }
  
    template <std::size_t Index>
    constexpr const subrange_view_type& get (void) const& noexcept
    {
      return std::get<Index> (m_subrange_views);
    }
  
    template <std::size_t Index>
    constexpr const subrange_view_type&& get (void) const&& noexcept
    {
      return std::get<Index> (m_subrange_views);
    }

#if __cpp_lib_three_way_comparison >= 201907L
  
    GCH_NODISCARD
    friend constexpr bool operator== (const partition_view& lhs, const partition_view& rhs)
    {
      return lhs.m_subrange_views == rhs.m_subrange_views;
    }
  
    GCH_NODISCARD
    friend constexpr auto operator<=> (const partition_view& lhs, const partition_view& rhs)
    {
      return lhs.m_subrange_views <=> rhs.m_subrange_views;
    }

#else
  
    GCH_NODISCARD
    friend bool operator== (const partition_view& lhs, const partition_view& rhs)
    {
      return lhs.m_subrange_views == rhs.m_subrange_views;
    }
  
    GCH_NODISCARD
    friend bool operator!= (const partition_view& lhs, const partition_view& rhs)
    {
      return lhs.m_subrange_views != rhs.m_subrange_views;
    }
  
    GCH_NODISCARD
    friend bool operator< (const partition_view& lhs, const partition_view& rhs)
    {
      return lhs.m_subrange_views < rhs.m_subrange_views;
    }
  
    GCH_NODISCARD
    friend bool operator<= (const partition_view& lhs, const partition_view& rhs)
    {
      return lhs.m_subrange_views <= rhs.m_subrange_views;
    }
  
    GCH_NODISCARD
    friend bool operator> (const partition_view& lhs, const partition_view& rhs)
    {
      return lhs.m_subrange_views > rhs.m_subrange_views;
    }
  
    GCH_NODISCARD
    friend bool operator>= (const partition_view& lhs, const partition_view& rhs)
    {
      return lhs.m_subrange_views >= rhs.m_subrange_views;
    }

#endif
  
  private:
    
    template <std::size_t Index>
    typename std::enable_if<(Index < partition_type::size ())>::type
    init_views (partition_type& p)
    {
      m_subrange_views[Index] = p.template subrange_view<Index> ();
      init_views<Index + 1> (p);
    }
  
    template <std::size_t Index>
    typename std::enable_if<(Index == partition_type::size ())>::type
    init_views (const partition_type& p) { }
    
    view_array m_subrange_views;
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
        { ! std::forward<T> (t) } -> boolean_testable_impl;
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
  
  template <std::size_t Index, typename Partition, std::size_t N>
  constexpr auto get (partition_view<Partition, N>& p) noexcept
    -> decltype (p.template get<Index> ())
  {
    return p.template get<Index> ();
  }
  
  template <std::size_t Index, typename Partition, std::size_t N>
  constexpr auto get (partition_view<Partition, N>&& p) noexcept
    -> decltype (p.template get<Index> ())
  {
    return p.template get<Index> ();
  }
  
  template <std::size_t Index, typename Partition, std::size_t N>
  constexpr auto get (const partition_view<Partition, N>& p) noexcept
    -> decltype (p.template get<Index> ())
  {
    return p.template get<Index> ();
  }
  
  template <std::size_t Index, typename Partition, std::size_t N>
  constexpr auto get (const partition_view<Partition, N>&& p) noexcept
    -> decltype (p.template get<Index> ())
  {
    return p.template get<Index> ();
  }
  
  template <typename Partition, std::size_t N>
  void swap (partition_view<Partition, N>& lhs, partition_view<Partition, N>& rhs)
    noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }
  
  template <typename Partition, std::size_t N>
  using const_partition_view = partition_view<const Partition, N>;
  
  namespace detail
  {
    template <typename DecayedPartition>
    struct partition_size_impl
      : std::integral_constant<std::size_t, DecayedPartition::num_subranges ()>
    { };
  
    template <std::size_t Index, typename DecayedPartition>
    struct partition_element_impl
    {
      using type = typename DecayedPartition::template subrange_type<Index>;
    };
  }
  
  template <typename Partition>
  struct partition_size
    : detail::partition_size_impl<typename std::decay<Partition>::type>
  { };
  
  template <std::size_t Index, typename Partition>
  struct partition_element
    : detail::partition_element_impl<Index, typename std::decay<Partition>::type>
  { };
  
  template <std::size_t Index, typename Partition>
  using partition_element_t = typename partition_element<Index, Partition>::type;
  
  namespace detail
  {
    template <std::size_t Accum, typename ...Partitions>
    struct total_subranges_impl;
  
    template <std::size_t Accum>
    struct total_subranges_impl<Accum>
      : std::integral_constant<std::size_t, Accum>
    { };
  
    template <std::size_t Accum, typename Partition, typename ...Partitions>
    struct total_subranges_impl<Accum, Partition, Partitions...>
      : total_subranges_impl<Accum + partition_size<Partition>::value, Partitions...>
    { };
  
    template <typename Void, typename ...Partitions>
    struct partition_container_type_impl
    { };
  
    template <typename Partition>
    struct partition_container_type_impl<void, Partition>
    {
      using type = typename Partition::container_type;
    };
  
    template <typename Partition, typename ...Partitions>
    struct partition_container_type_impl<
      typename std::enable_if<std::is_same<typename Partition::container_type,
        typename partition_container_type_impl<void, Partitions...>::type>::value>::type,
      Partition, Partitions...>
      : partition_container_type_impl<void, Partitions...>
    { };
  
    template <typename Void, typename ...Partitions>
    struct partition_data_type_impl
    { };
  
    template <typename Partition>
    struct partition_data_type_impl<void, Partition>
    {
      using type = typename Partition::data_type;
    };
  
    template <typename Partition, typename ...Partitions>
    struct partition_data_type_impl<
      typename std::enable_if<std::is_same<typename Partition::data_type,
        typename partition_data_type_impl<void, Partitions...>::type>::value>::type,
      Partition, Partitions...>
      : partition_data_type_impl<void, Partitions...>
    { };
  
    template <std::size_t M, typename Partition>
    struct partition_resized_type_impl;
    
    template <std::size_t M, typename T, std::size_t N, typename Container,
              template <typename, std::size_t, typename> class PartitionT>
    struct partition_resized_type_impl<M, PartitionT<T, N, Container>>
    {
      using type = PartitionT<T, M, Container>;
    };
  
    template <typename ...Partitions>
    struct partition_cat_type_impl;
  
    template <typename Partition, typename ...Partitions>
    struct partition_cat_type_impl<Partition, Partitions...>
      : partition_resized_type_impl<total_subranges_impl<0, Partition, Partitions...>::value,
                                    Partition>
    { };
    
  };
  
  template <typename ...Partitions>
  struct total_subranges
    : detail::total_subranges_impl<0, typename std::decay<Partitions>::type...>
  { };
  
  template <typename ...Partitions>
  struct partition_container_type
    : detail::partition_container_type_impl<void, typename std::decay<Partitions>::type...>
  { };
  
  template <typename ...Partitions>
  struct partition_data_type
    : detail::partition_data_type_impl<void, typename std::decay<Partitions>::type...>
  { };
  
  template <std::size_t M, typename Partition>
  struct partition_resized_type
    : detail::partition_resized_type_impl<M, typename std::decay<Partition>::type>
  { };
  
  template <typename ...Partitions>
  struct partition_cat_type
    : detail::partition_cat_type_impl<typename std::decay<Partitions>::type...>
  { };
  
  template <typename ...Partitions>
  using partition_container_type_t = typename partition_container_type<Partitions...>::type;
  
  template <typename ...Partitions>
  using partition_data_type_t = typename partition_data_type<Partitions...>::type;
  
  template <std::size_t M, typename Partition>
  using partition_resized_type_t = typename partition_resized_type<M, Partition>::type;
  
  template <typename ...Partitions>
  using partition_cat_type_t = typename partition_cat_type<Partitions...>::type;
  
  template <typename ...Partitions>
  partition_cat_type_t<Partitions...> partition_cat (Partitions&&... ps)
  {
    return { std::forward<Partitions> (ps)... };
  }
  
  template <std::size_t Index, typename Partition>
  constexpr partition_element_t<Index, Partition>&
  get_subrange (Partition& p) noexcept
  {
    return static_cast<partition_element_t<Index, Partition>&> (p);
  }
  
  template <std::size_t Index, typename Partition>
  constexpr const partition_element_t<Index, Partition>&
  get_subrange (const Partition& p) noexcept
  {
    return static_cast<const partition_element_t<Index, Partition>&> (p);
  }
  
  template <std::size_t Index, typename Partition>
  constexpr partition_element_t<Index, Partition>&&
  get_subrange (Partition&& p) noexcept
  {
    return static_cast<partition_element_t<Index, Partition>&&> (p);
  }
  
  template <std::size_t Index, typename Partition>
  constexpr const partition_element_t<Index, Partition>&&
  get_subrange (const Partition&& p) noexcept
  {
    return static_cast<const partition_element_t<Index, Partition>&&> (p);
  }

#ifdef GCH_PARTITION_ITERATOR

#  define GCH_PARTITION_ITERATOR
  
  template <typename Partition>
  class partition_iterator
  {
    template <typename IndexSequence>
    struct find_variant;
    
    template <std::size_t ...Indices>
    struct find_variant<std::index_sequence<Indices...>>
    {
      using type = std::variant<std::reference_wrapper<
        typename Partition::template subrange_type<Indices>>...>;
    };
  
  public:
    
    using subrange_indices = std::make_index_sequence<Partition::size ()>;
    using variant_subrange = typename find_variant<subrange_indices>::type;
    
    partition_iterator            (void)                          = default;
    partition_iterator            (const partition_iterator&)     = default;
    partition_iterator            (partition_iterator&&) noexcept = default;
    partition_iterator& operator= (const partition_iterator&)     = default;
    partition_iterator& operator= (partition_iterator&&) noexcept = default;
    ~partition_iterator           (void)                          = default;
    
    constexpr partition_iterator (Partition& p, std::size_t idx)
      : m_partition (&p),
        m_idx (idx)
    { }
    
    template <typename NonConst,
              typename = std::enable_if_t<
                std::is_same<std::remove_const_t<Partition>, NonConst>::value>>
    constexpr /* implicit */ partition_iterator (const partition_iterator<NonConst>& other) noexcept
      : m_partition (other.m_partition),
        m_idx (other.m_idx)
    { }
    
    template <typename NonConst,
              typename = std::enable_if_t<
                std::is_same<std::remove_const_t<Partition>, NonConst>::value>>
    partition_iterator& operator= (const partition_iterator<NonConst>& other) noexcept
    {
      m_partition = other.m_partition;
      m_idx = other.m_idx;
      return *this;
    }
    
    friend partition_iterator<std::add_const_t<Partition>>;
    
    partition_iterator& operator++ (void)
    {
      ++m_idx;
      return *this;
    }
    
    partition_iterator& operator++ (int)
    {
      partition_iterator tmp = *this;
      ++m_idx;
      return *this;
    }
    
    partition_iterator& operator-- (void)
    {
      --m_idx;
      return *this;
    }
    
    partition_iterator& operator-- (int)
    {
      partition_iterator tmp = *this;
      --m_idx;
      return *this;
    }
    
    constexpr bool operator== (const partition_iterator& other) const
    {
      return m_partition == other.m_partition && m_idx == other.m_idx;
    }
    
    constexpr bool operator!= (const partition_iterator& other) const {
      return ! operator== (other);
    }
    
    constexpr variant_subrange operator* (void) const
    {
      return cast_subrange (*m_partition, m_idx);
    }
  
  private:
    
    template <typename IndexSequence>
    struct subrange_map;
    
    template <std::size_t ...Indices>
    struct subrange_map<std::index_sequence<Indices...>>
    {
      template <std::size_t Index>
      static constexpr variant_subrange make_variant (Partition& p)
      {
        return std::ref (get_subrange<Index> (p));
      }
      
      using variant_functor = variant_subrange (*) (Partition&);
      static constexpr auto size = sizeof...(Indices);
      
      static constexpr std::array<variant_functor, size>
        variant_functor_map = { &make_variant<Indices>... };
    };
    
    static constexpr variant_subrange cast_subrange (Partition& p, std::size_t idx)
    {
      return subrange_map<subrange_indices>::variant_functor_map[idx] (p);
    }
    
    Partition * m_partition;
    std::size_t m_idx;
  };
  
  namespace detail
  {
    
    template <typename ...Fs>
    struct overload_wrapper : Fs ...
    {
      template <typename ...Ts,
                typename = std::enable_if_t<std::conjunction_v<
                  std::is_same<std::decay_t<Ts>, Fs>...>>>
      constexpr overload_wrapper (Ts&& ... fs) noexcept
        : Fs (std::forward<Ts> (fs))...
      { }
      
      template <typename T>
      constexpr auto call (T&& t)
      {
        return operator() (std::forward<T> (t));
      }
      
      using Fs::operator()...;
    };
    
    template <typename Partition, typename Derived, std::size_t Idx>
    struct subrange_overload;
    
    template <typename Partition, template <typename...> class DerivedT,
      std::size_t Idx, typename ...Fs>
    struct subrange_overload<Partition, DerivedT<Fs...>, Idx>
    {
      using derived_type = DerivedT<Fs...>;
      using subrange_type = typename Partition::template subrange_type<Idx>;
      
      constexpr auto operator() (std::reference_wrapper<subrange_type>& r)
      {
        return wrapper_cast (this)->call (r.get ());
      }
      
      constexpr auto operator() (std::reference_wrapper<subrange_type>&& r)
      {
        return wrapper_cast (this)->call (r.get ());
      }
      
      constexpr auto operator() (const std::reference_wrapper<subrange_type>& r)
      {
        return wrapper_cast (this)->call (r.get ());
      }
      
      constexpr auto operator() (const std::reference_wrapper<subrange_type>&& r)
      {
        return wrapper_cast (this)->call (r.get ());
      }
      
      static constexpr auto up_cast (subrange_overload* ptr)
      {
        return static_cast<derived_type *> (ptr);
      }
      
      static constexpr auto up_cast (const subrange_overload* ptr)
      {
        return static_cast<const derived_type *> (ptr);
      }
      
      static constexpr auto wrapper_cast (subrange_overload* ptr)
      {
        return static_cast<typename derived_type::wrapper_type *> (up_cast (ptr));
      }
      
      static constexpr auto wrapper_cast (const subrange_overload* ptr)
      {
        return static_cast<const typename derived_type::wrapper_type *> (up_cast (ptr));
      }
    };
    
    
    template <typename Partition, typename Derived, typename IndexSequence>
    struct partition_overload_impl;
    
    template <typename Partition, typename Derived, std::size_t ...Indices>
    struct partition_overload_impl<Partition, Derived, std::index_sequence<Indices...>>
      : subrange_overload<Partition, Derived, Indices>...
    {
      using subrange_overload<Partition, Derived, Indices>::operator()...;
    };
    
  }
  
  template <typename Partition, typename ...Fs>
  struct partition_overloader
    : detail::overload_wrapper<Fs>...,
      detail::partition_overload_impl<Partition, partition_overloader<Partition, Fs...>,
                                      std::make_index_sequence<Partition::size ()>>
  {
    using detail::overload_wrapper<Fs...>::overload_wrapper;
    using wrapper_type = detail::overload_wrapper<Fs...>;
  };
  
  template <typename Partition, typename ...Fs,
            typename = std::enable_if_t<std::conjunction_v<
              std::negation<std::is_same<std::decay_t<Fs>, Partition>>...>>>
  constexpr auto partition_overload (Fs&&... fs)
  {
    return partition_overloader<Partition, std::decay_t<Fs>...> (std::forward<Fs> (fs)...);
  }
  
  template <typename Partition, typename ...Fs>
  constexpr auto partition_overload (const Partition&, Fs&&... fs)
  {
    return partition_overloader<Partition, std::decay_t<Fs>...> (std::forward<Fs> (fs)...);
  }

#endif
  
};

namespace std
{
  template <typename Container, std::size_t N>
  struct tuple_size<gch::dependent_partition_view<Container, N>>
    : public std::integral_constant<std::size_t, N>
  { };
  
  template <typename Partition, std::size_t N>
  struct tuple_size<gch::partition_view<Partition, N>>
    : public std::integral_constant<std::size_t, N>
  { };
  
  template <std::size_t I, typename Container, std::size_t N>
  struct tuple_element<I, gch::dependent_partition_view<Container, N>>
  {
    using type = typename gch::dependent_partition_view<Container, N>::subrange_type;
  };
  
  template <std::size_t I, typename Partition, std::size_t N>
  struct tuple_element<I, gch::partition_view<Partition, N>>
  {
    using type = typename gch::partition_view<Partition, N>::subrange_type;
  };
}

#undef GCH_CPP14_CONSTEXPR
#undef GCH_CPP17_CONSTEXPR
#undef GCH_CPP20_CONSTEXPR
#undef GCH_NODISCARD

#endif // PARTITION_HPP
