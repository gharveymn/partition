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

#ifndef GCH_INLINE_VARS
#  if __cpp_inline_variables >= 201606
#    define GCH_INLINE_VARS inline
#  else
#    define GCH_INLINE_VARS
#  endif
#endif

namespace gch
{
  
  namespace adl
  {
    using std::swap;
    namespace resolved
    {
      template <typename It>
      auto swap (It& lhs, It& rhs) noexcept (noexcept (swap (lhs, rhs)))
      -> decltype (swap (lhs, rhs));
    }
  }
  
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
  
  namespace detail
  {
    template <std::size_t Accum, typename ...Partitions>
    struct total_subranges_impl;
  
    template <std::size_t Accum>
    struct total_subranges_impl<Accum>
      : std::integral_constant<std::size_t, Accum>
    { };
  
    template <std::size_t Accum, typename Partition, typename ...Rest>
    struct total_subranges_impl<Accum, Partition, Rest...>
      : total_subranges_impl<Accum + partition_size<Partition>::value, Rest...>
    { };
  
    template <typename Void, typename ...DecayedPartitions>
    struct partition_container_type_impl
    { };
  
    template <typename DecayedPartition>
    struct partition_container_type_impl<void, DecayedPartition>
    {
      using type = typename DecayedPartition::container_type;
    };
  
    template <typename DecayedPartition, typename ...Rest>
    struct partition_container_type_impl<
      typename std::enable_if<std::is_same<typename DecayedPartition::container_type,
        typename partition_container_type_impl<void, Rest...>::type>::value>::type,
      DecayedPartition, Rest...>
      : partition_container_type_impl<void, Rest...>
    { };
  
    template <typename Void, typename ...DecayedPartitions>
    struct partition_data_type_impl
    { };
  
    template <typename DecayedPartition>
    struct partition_data_type_impl<void, DecayedPartition>
    {
      using type = typename DecayedPartition::data_type;
    };
  
    template <typename DecayedPartition, typename ...Rest>
    struct partition_data_type_impl<
      typename std::enable_if<std::is_same<typename DecayedPartition::data_type,
        typename partition_data_type_impl<void, Rest...>::type>::value>::type,
      DecayedPartition, Rest...>
      : partition_data_type_impl<void, Rest...>
    { };
  
    template <std::size_t M, typename DecayedPartition>
    struct partition_resized_type_impl;
    
    template <std::size_t M, typename T, std::size_t N, typename Container,
              template <typename, std::size_t, typename> class DecayedPartitionT>
    struct partition_resized_type_impl<M, DecayedPartitionT<T, N, Container>>
    {
      using type = DecayedPartitionT<T, M, Container>;
    };
    
    template <typename ...>
    using void_t = void;
  
    template <typename Void1, typename Void2, typename ...Partitions>
    struct partition_cat_type_impl
    { };
  
    template <typename DecayedPartition, typename ...Rest>
    struct partition_cat_type_impl<void_t<partition_data_type_impl<void, DecayedPartition>>,
                                   void_t<partition_container_type_impl<void, DecayedPartition>>,
                                   DecayedPartition, Rest...>
      : partition_resized_type_impl<total_subranges_impl<0, DecayedPartition, Rest...>::value,
                                    DecayedPartition>
    { };
    
    template <typename DecayedSubrange>
    struct parent_partition_type_impl
    {
      using type = typename DecayedSubrange::partition_type;
    };
  
    template <typename DecayedSubrange>
    struct next_subrange_type_impl
    {
      using type = typename DecayedSubrange::next_type;
    };
  
    template <typename DecayedSubrange>
    struct prev_subrange_type_impl
    {
      using type = typename DecayedSubrange::prev_type;
    };
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
    : detail::partition_cat_type_impl<void, void, typename std::decay<Partitions>::type...>
  { };
  
  template <typename Subrange>
  struct parent_partition_type
    : detail::parent_partition_type_impl<typename std::decay<Subrange>::type>
  { };
  
  template <typename Subrange>
  struct next_subrange_type
    : detail::next_subrange_type_impl<typename std::decay<Subrange>::type>
  { };
  
  template <typename Subrange>
  struct prev_subrange_type
    : detail::prev_subrange_type_impl<typename std::decay<Subrange>::type>
  { };
  
  template <std::size_t Index, typename Partition>
  using partition_element_t = typename partition_element<Index, Partition>::type;
  
  template <typename ...Partitions>
  using partition_container_t = typename partition_container_type<Partitions...>::type;
  
  template <typename ...Partitions>
  using partition_data_t = typename partition_data_type<Partitions...>::type;
  
  template <std::size_t M, typename Partition>
  using partition_resized_t = typename partition_resized_type<M, Partition>::type;
  
  template <typename ...Partitions>
  using partition_cat_t = typename partition_cat_type<Partitions...>::type;
  
  template <typename Subrange>
  using parent_partition_t = typename parent_partition_type<Subrange>::type;
  
  template <typename Subrange>
  using next_subrange_t = typename next_subrange_type<Subrange>::type;
  
  template <typename Subrange>
  using prev_subrange_t = typename prev_subrange_type<Subrange>::type;
  
  template <typename ...Partitions>
  partition_cat_t<Partitions...> partition_cat (Partitions&&... ps)
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
  
  template <typename Subrange>
  constexpr next_subrange_t<Subrange>&
  next_subrange (Subrange& p) noexcept
  {
    return static_cast<next_subrange_t<Subrange>&> (p);
  }
  
  template <typename Subrange>
  constexpr const next_subrange_t<Subrange>&
  next_subrange (const Subrange& p) noexcept
  {
    return static_cast<const next_subrange_t<Subrange>&> (p);
  }
  
  template <typename Subrange>
  constexpr next_subrange_t<Subrange>&&
  next_subrange (Subrange&& p) noexcept
  {
    return static_cast<next_subrange_t<Subrange>&&> (p);
  }
  
  template <typename Subrange>
  constexpr const next_subrange_t<Subrange>&&
  next_subrange (const Subrange&& p) noexcept
  {
    return static_cast<const next_subrange_t<Subrange>&&> (p);
  }
  
  template <typename Subrange>
  constexpr prev_subrange_t<Subrange>&
  prev_subrange (Subrange& p) noexcept
  {
    return static_cast<prev_subrange_t<Subrange>&> (p);
  }
  
  template <typename Subrange>
  constexpr const prev_subrange_t<Subrange>&
  prev_subrange (const Subrange& p) noexcept
  {
    return static_cast<const prev_subrange_t<Subrange>&> (p);
  }
  
  template <typename Subrange>
  constexpr prev_subrange_t<Subrange>&&
  prev_subrange (Subrange&& p) noexcept
  {
    return static_cast<prev_subrange_t<Subrange>&&> (p);
  }
  
  template <typename Subrange>
  constexpr const prev_subrange_t<Subrange>&&
  prev_subrange (const Subrange&& p) noexcept
  {
    return static_cast<const prev_subrange_t<Subrange>&&> (p);
  }
  
  template <typename Subrange>
  constexpr parent_partition_t<Subrange>&
  get_partition (Subrange& p) noexcept
  {
    return static_cast<parent_partition_t<Subrange>&> (p);
  }
  
  template <typename Subrange>
  constexpr const parent_partition_t<Subrange>&
  get_partition (const Subrange& p) noexcept
  {
    return static_cast<const parent_partition_t<Subrange>&> (p);
  }
  
  template <typename Subrange>
  constexpr parent_partition_t<Subrange>&&
  get_partition (Subrange&& p) noexcept
  {
    return static_cast<parent_partition_t<Subrange>&&> (p);
  }
  
  template <typename Subrange>
  constexpr const parent_partition_t<Subrange>&&
  get_partition (const Subrange&& p) noexcept
  {
    return static_cast<const parent_partition_t<Subrange>&&> (p);
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
  template <typename Partition, std::size_t N>
  struct tuple_size<gch::partition_view<Partition, N>>
    : public std::integral_constant<std::size_t, N>
  { };
  
  template <std::size_t I, typename Partition, std::size_t N>
  struct tuple_element<I, gch::partition_view<Partition, N>>
  {
    using type = typename gch::partition_view<Partition, N>::subrange_type;
  };
}

#endif // PARTITION_HPP
