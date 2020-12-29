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

#if __has_include(<variant>)
#  include <variant>
#  if __cpp_lib_variant >= 201606L
#    ifndef GCH_HAS_VARIANT
#      define GCH_HAS_VARIANT
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

#if __cpp_impl_three_way_comparison >= 201907L
#  ifndef GCH_IMPL_THREE_WAY_COMPARISON
#    define GCH_IMPL_THREE_WAY_COMPARISON
#  endif
#  if __has_include(<compare>)
#    include <compare>
#    if __cpp_lib_three_way_comparison >= 201907L
#      ifndef GCH_LIB_THREE_WAY_COMPARISON
#        define GCH_LIB_THREE_WAY_COMPARISON
#      endif
#    endif
#  endif
#endif

#ifndef GCH_TEMPLATE_AUTO
#  if __cpp_nontype_template_parameter_auto >= 201606L
#    define GCH_TEMPLATE_AUTO
#  endif
#endif

#ifndef GCH_VARIABLE_TEMPLATES
#  if __cpp_variable_templates >= 201304L
#  define GCH_VARIABLE_TEMPLATES
#  endif
#endif

#ifndef GCH_INLINE_VAR
#  if __cpp_inline_variables >= 201606L
#    define GCH_INLINE_VAR inline
#  else
#    define GCH_INLINE_VAR
#  endif
#endif

namespace gch
{

  namespace detail
  {
    template <typename From, typename To>
    using match_ref_t = typename std::conditional<std::is_lvalue_reference<From>::value,
                                                  typename std::add_lvalue_reference<To>::type,
                                                  typename std::conditional<
                                                    std::is_rvalue_reference<From>::value,
                                                    typename std::add_rvalue_reference<To>::type,
                                                    To>::type
                                                 >::type;
  }

  template <typename Partition, std::size_t Index, typename Enable = void>
  class partition_subrange;

  template <typename Partition>
  struct partition_traits;

  template <template <typename, std::size_t, typename> class PartitionT,
            typename T, std::size_t N, typename Container>
  struct partition_traits<PartitionT<T, N, Container>>
  {
    using partition_type  = PartitionT<T, N, Container>;

    using value_type      = T;
    using container_type  = Container;

    static constexpr std::size_t num_subranges = N;

    using data_iterator                = typename container_type::iterator;
    using data_const_iterator          = typename container_type::const_iterator;
    using data_reverse_iterator        = typename container_type::reverse_iterator;
    using data_const_reverse_iterator  = typename container_type::const_reverse_iterator;
    using data_reference               = typename container_type::reference;
    using data_const_reference         = typename container_type::const_reference;
    using data_size_type               = typename container_type::size_type;
    using data_difference_type         = typename container_type::difference_type;
    using data_value_type              = typename container_type::value_type;
    using data_allocator_type          = typename container_type::allocator_type;
  };

  template <typename Partition>
  struct partition_value_type
  {
    using type = typename partition_traits<Partition>::value_type;
  };

  template <typename ...Partitions>
  using partition_value_t = typename partition_value_type<Partitions...>::type;

  template <typename Partition>
  struct partition_container_type
  {
    using type = typename partition_traits<Partition>::container_type;
  };

  template <typename ...Partitions>
  using partition_container_t = typename partition_container_type<Partitions...>::type;

  template <typename Partition>
  struct partition_size
    : std::integral_constant<std::size_t, partition_traits<Partition>::num_subranges>
  { };

  namespace detail
  {
    template <typename T,
              typename U = typename std::remove_cv<T>::type,
              typename = typename std::enable_if<std::is_same<T, U>::value>::type,
              std::size_t = partition_size<T>::value>
    using enable_if_has_partition_size = T;
  }

  template <typename Partition>
  struct partition_size<const detail::enable_if_has_partition_size<Partition>>
    : partition_size<Partition>
  { };

  template <typename Partition>
  struct partition_size<volatile detail::enable_if_has_partition_size<Partition>>
    : partition_size<Partition>
  { };

  template <typename Partition>
  struct partition_size<const volatile detail::enable_if_has_partition_size<Partition>>
    : partition_size<Partition>
  { };

#ifdef GCH_VARIABLE_TEMPLATES
  template <typename Partition>
  GCH_INLINE_VAR constexpr std::size_t partition_size_v = partition_size<Partition>::value;
#endif

  template <std::size_t I, typename Partition>
  struct partition_element;

  template <std::size_t Index, typename Partition>
  using partition_element_t = typename partition_element<Index, Partition>::type;

  template <std::size_t I, typename Partition>
  struct partition_element
  {
    using type = partition_subrange<Partition, I>;
  };

  template <std::size_t I, typename Partition>
  struct partition_element<I, const Partition>
  {
    using type = typename std::add_const<partition_element_t<I, Partition>>::type;
  };

  template <std::size_t I, typename Partition>
  struct partition_element<I, volatile Partition>
  {
    using type = typename std::add_volatile<partition_element_t<I, Partition>>::type;
  };

  template <std::size_t I, typename Partition>
  struct partition_element<I, const volatile Partition>
  {
    using type = typename std::add_cv<partition_element_t<I, Partition>>::type;
  };

  template <typename ...Partitions>
  struct total_partition_size;

  template <typename P>
  struct total_partition_size<P>
    : std::integral_constant<std::size_t, partition_size<P>::value>
  { };

  template <typename P, typename ...Rest>
  struct total_partition_size<P, Rest...>
    : std::integral_constant<std::size_t,
                             partition_size<P>::value + total_partition_size<Rest...>::value>
  { };

#ifdef GCH_VARIABLE_TEMPLATES
  template <typename ...Ps>
  GCH_INLINE_VAR constexpr std::size_t total_partition_size_v = total_partition_size<Ps...>::value;
#endif

  template <typename Partition, std::size_t M>
  struct partition_resized;

  template <typename Partition, std::size_t M>
  using partition_resized_t = typename partition_resized<Partition, M>::type;

  template <template <typename, std::size_t, typename> class PartitionT,
            typename T, std::size_t N, typename Container, std::size_t M>
  struct partition_resized<PartitionT<T, N, Container>, M>
  {
    using type = PartitionT<T, M, Container>;
  };

  template <typename Partition, std::size_t M>
  struct partition_resized<const Partition, M>
  {
    using type = typename std::add_const<partition_resized_t<Partition, M>>::type;
  };

  template <typename Partition, std::size_t M>
  struct partition_resized<volatile Partition, M>
  {
    using type = typename std::add_volatile<partition_resized_t<Partition, M>>::type;
  };

  template <typename Partition, std::size_t M>
  struct partition_resized<const volatile Partition, M>
  {
    using type = typename std::add_cv<partition_resized_t<Partition, M>>::type;
  };

  template <typename P, typename Q, typename = void>
  struct is_compatible_partition
    : std::false_type
  { };

  template <typename P, typename Q>
  struct is_compatible_partition<P, Q,
    typename std::enable_if<std::is_same<partition_value_t<P>,
                                         partition_value_t<Q>>::value &&
                            std::is_same<partition_container_t<P>,
                                         partition_container_t<Q>>::value>::type>
    : std::true_type
  { };

  namespace detail
  {

    template <typename P, typename Q,
              typename = typename std::enable_if<is_compatible_partition<P, Q>::value>::type>
    using enable_if_compatible_partition = Q;
  };

  template <typename ...Partitions>
  struct partition_cat_type
  { };

  template <typename Partition, typename ...Rest>
  struct partition_cat_type<Partition, detail::enable_if_compatible_partition<Partition, Rest>...>
  {
    using type = partition_resized_t<Partition, total_partition_size<Partition, Rest...>::value>;
  };

  template <typename ...Partitions>
  using partition_cat_t = typename partition_cat_type<Partitions...>::type;

  template <typename Subrange>
  struct subrange_traits;

  template <typename Partition, std::size_t Index>
  struct subrange_traits<partition_subrange<Partition, Index>>
  {

    static constexpr std::size_t index = Index;

    using partition_type = Partition;
    using subrange_type  = partition_subrange<Partition, Index>;

    using container_type         = typename partition_traits<partition_type>::container_type;
    using iterator               = typename container_type::iterator;
    using const_iterator         = typename container_type::const_iterator;
    using reverse_iterator       = typename container_type::reverse_iterator;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;
    using reference              = typename container_type::reference;
    using const_reference        = typename container_type::const_reference;
    using size_type              = typename container_type::size_type;
    using difference_type        = typename container_type::difference_type;
    using value_type             = typename container_type::value_type;
    using allocator_type         = typename container_type::allocator_type;

  };

  template <typename Subrange>
  struct partition_type
  {
    using type = typename subrange_traits<Subrange>::partition_type;
  };

  template <typename Subrange>
  using partition_type_t = typename partition_type<Subrange>::type;

  template <typename Subrange, std::ptrdiff_t Offset = 1, typename Enable = void>
  struct next_subrange_type { };

  template <typename Subrange>
  struct next_subrange_type<Subrange, 0>
  {
    using type = Subrange;
  };

  template <typename Partition, std::size_t Index, std::ptrdiff_t Offset>
  struct next_subrange_type<partition_subrange<Partition, Index>, Offset,
    typename std::enable_if<
      ((Offset > 0) && (static_cast<std::size_t> (Offset) <=
                        (partition_size<Partition>::value - Index))) ||
      ((Offset < 0) && (static_cast<std::size_t> (-Offset) <= Index))>::type>
  {
    using type = partition_element_t<Index + static_cast<std::size_t> (Offset), Partition>;
  };

  template <typename Subrange, std::ptrdiff_t Offset = 1>
  struct prev_subrange_type
    : next_subrange_type<Subrange, -Offset>
  { };

  template <typename Subrange, std::ptrdiff_t Offset = 1>
  using next_subrange_t = typename next_subrange_type<Subrange, Offset>::type;

  template <typename Subrange, std::ptrdiff_t Offset = 1>
  using prev_subrange_t = typename prev_subrange_type<Subrange, Offset>::type;

  template <typename ...Partitions>
  partition_cat_t<typename std::remove_reference<Partitions>::type...>
  partition_cat (Partitions&&... ps)
  {
    return { std::forward<Partitions> (ps)... };
  }

  template <std::size_t I, template <typename, std::size_t, typename> class PartitionT,
            typename T, std::size_t N, typename Container>
  constexpr partition_element_t<I, PartitionT<T, N, Container>>&
  get_subrange (PartitionT<T, N, Container>& p) noexcept
  {
    return static_cast<partition_element_t<I, PartitionT<T, N, Container>>&> (p);
  }

  template <std::size_t I, template <typename, std::size_t, typename> class PartitionT,
            typename T, std::size_t N, typename Container>
  constexpr const partition_element_t<I, PartitionT<T, N, Container>>&
  get_subrange (const PartitionT<T, N, Container>& p) noexcept
  {
    return static_cast<const partition_element_t<I, PartitionT<T, N, Container>>&> (p);
  }

  template <std::size_t I, template <typename, std::size_t, typename> class PartitionT,
            typename T, std::size_t N, typename Container>
  constexpr partition_element_t<I, PartitionT<T, N, Container>>&&
  get_subrange (PartitionT<T, N, Container>&& p) noexcept
  {
    return static_cast<partition_element_t<I, PartitionT<T, N, Container>>&&> (p);
  }

  template <std::size_t I, template <typename, std::size_t, typename> class PartitionT,
            typename T, std::size_t N, typename Container>
  constexpr const partition_element_t<I, PartitionT<T, N, Container>>&&
  get_subrange (const PartitionT<T, N, Container>&& p) noexcept
  {
    return static_cast<const partition_element_t<I, PartitionT<T, N, Container>>&&> (p);
  }

#ifdef GCH_TEMPLATE_AUTO

  template <auto EnumIndex, typename Partition,
            std::enable_if_t<
              std::is_enum_v<decltype (EnumIndex)> &&
              std::is_convertible_v<std::underlying_type_t<decltype (EnumIndex)>,
                                    std::size_t>> * = nullptr>
  constexpr decltype (auto) get_subrange (Partition&& p) noexcept
  {
    return get_subrange<static_cast<std::size_t> (EnumIndex)> (std::forward<Partition> (p));
  }

  template <auto Index, typename Partition,
            std::enable_if_t<! std::is_enum_v<decltype (Index)> &&
                             ! std::is_same_v<decltype (Index), std::size_t> &&
                             std::is_convertible_v<decltype (Index), std::size_t>> * = nullptr>
  constexpr decltype (auto) get_subrange (Partition&& p) noexcept
  {
    return get_subrange<static_cast<std::size_t> (Index)> (std::forward<Partition> (p));
  }

#else

  template <typename T, T Index, typename Partition,
            typename std::enable_if<
                std::is_enum<T>::value &&
                std::is_convertible<typename std::underlying_type<T>::type,
                                    std::size_t>::value>::type * = nullptr>
  constexpr auto get_subrange (Partition&& p) noexcept
    -> decltype (get_subrange<static_cast<std::size_t> (Index)> (std::forward<Partition> (p)))
  {
    return get_subrange<static_cast<std::size_t> (Index)> (std::forward<Partition> (p));
  }

  template <typename T, T Index, typename Partition,
    typename std::enable_if<! std::is_enum<T>::value &&
                            ! std::is_same<T, std::size_t>::value &&
                            std::is_convertible<T, std::size_t>::value>::type * = nullptr>
  constexpr auto get_subrange (Partition&& p) noexcept
    -> decltype (get_subrange<static_cast<std::size_t> (Index)> (std::forward<Partition> (p)))
  {
    return get_subrange<static_cast<std::size_t> (Index)> (std::forward<Partition> (p));
  }

#endif

  template <typename Partition, std::size_t Index, std::ptrdiff_t Offset = 1>
  constexpr next_subrange_t<partition_subrange<Partition, Index>, Offset>&
  next_subrange (partition_subrange<Partition, Index>& p) noexcept
  {
    return static_cast<next_subrange_t<partition_subrange<Partition, Index>, Offset>&> (p);
  }

  template <typename Partition, std::size_t Index, std::ptrdiff_t Offset = 1>
  constexpr const next_subrange_t<partition_subrange<Partition, Index>, Offset>&
  next_subrange (const partition_subrange<Partition, Index>& p) noexcept
  {
    return static_cast<const next_subrange_t<partition_subrange<Partition, Index>, Offset>&> (p);
  }

  template <typename Partition, std::size_t Index, std::ptrdiff_t Offset = 1>
  constexpr next_subrange_t<partition_subrange<Partition, Index>, Offset>&&
  next_subrange (partition_subrange<Partition, Index>&& p) noexcept
  {
    return static_cast<next_subrange_t<partition_subrange<Partition, Index>, Offset>&&> (p);
  }

  template <typename Partition, std::size_t Index, std::ptrdiff_t Offset = 1>
  constexpr const next_subrange_t<partition_subrange<Partition, Index>, Offset>&&
  next_subrange (const partition_subrange<Partition, Index>&& p) noexcept
  {
    return static_cast<const next_subrange_t<partition_subrange<Partition, Index>, Offset>&&> (p);
  }

  template <typename Partition, std::size_t Index, std::ptrdiff_t Offset = 1>
  constexpr prev_subrange_t<partition_subrange<Partition, Index>, Offset>&
  prev_subrange (partition_subrange<Partition, Index>& p) noexcept
  {
    return static_cast<prev_subrange_t<partition_subrange<Partition, Index>, Offset>&> (p);
  }

  template <typename Partition, std::size_t Index, std::ptrdiff_t Offset = 1>
  constexpr const prev_subrange_t<partition_subrange<Partition, Index>, Offset>&
  prev_subrange (const partition_subrange<Partition, Index>& p) noexcept
  {
    return static_cast<const prev_subrange_t<partition_subrange<Partition, Index>, Offset>&> (p);
  }

  template <typename Partition, std::size_t Index, std::ptrdiff_t Offset = 1>
  constexpr prev_subrange_t<partition_subrange<Partition, Index>, Offset>&&
  prev_subrange (partition_subrange<Partition, Index>&& p) noexcept
  {
    return static_cast<prev_subrange_t<partition_subrange<Partition, Index>, Offset>&&> (p);
  }

  template <typename Partition, std::size_t Index, std::ptrdiff_t Offset = 1>
  constexpr const prev_subrange_t<partition_subrange<Partition, Index>, Offset>&&
  prev_subrange (const partition_subrange<Partition, Index>&& p) noexcept
  {
    return static_cast<const prev_subrange_t<partition_subrange<Partition, Index>, Offset>&&> (p);
  }

  template <typename Partition, std::size_t Index>
  constexpr partition_type_t<partition_subrange<Partition, Index>>&
  get_partition (partition_subrange<Partition, Index>& p) noexcept
  {
    return static_cast<partition_type_t<partition_subrange<Partition, Index>>&> (p);
  }

  template <typename Partition, std::size_t Index>
  constexpr const partition_type_t<partition_subrange<Partition, Index>>&
  get_partition (const partition_subrange<Partition, Index>& p) noexcept
  {
    return static_cast<const partition_type_t<partition_subrange<Partition, Index>>&> (p);
  }

  template <typename Partition, std::size_t Index>
  constexpr partition_type_t<partition_subrange<Partition, Index>>&&
  get_partition (partition_subrange<Partition, Index>&& p) noexcept
  {
    return static_cast<partition_type_t<partition_subrange<Partition, Index>>&&> (p);
  }

  template <typename Partition, std::size_t Index>
  constexpr const partition_type_t<partition_subrange<Partition, Index>>&&
  get_partition (const partition_subrange<Partition, Index>&& p) noexcept
  {
    return static_cast<const partition_type_t<partition_subrange<Partition, Index>>&&> (p);
  }

  template <typename Iterator>
  class subrange_view
  {
  public:
    using iterator  = Iterator;

    using iter      = Iterator;
    using iter_diff = typename std::iterator_traits<iter>::difference_type;
    using iter_val  = typename std::iterator_traits<iter>::value_type;
    using iter_ptr  = typename std::iterator_traits<iter>::pointer;
    using iter_ref  = typename std::iterator_traits<iter>::reference;

    subrange_view            (void)                     = default;
    subrange_view            (const subrange_view&)     = default;
    subrange_view            (subrange_view&&) noexcept = default;
    subrange_view& operator= (const subrange_view&)     = default;
    subrange_view& operator= (subrange_view&&) noexcept = default;
    ~subrange_view           (void)                     = default;

    subrange_view (iter first, iter last)
      : m_first (first),
        m_last  (last)
    { }

    iter  begin (void) const noexcept { return m_first; }
    iter  end   (void) const noexcept { return m_last;  }

    iter_ref front (void) const noexcept { return *begin (); }
    iter_ref back  (void) const noexcept { return *(--end ()); }

    GCH_NODISCARD std::size_t size  (void) const noexcept { return std::distance (m_first, m_last); }
    GCH_NODISCARD bool        empty (void) const noexcept { return m_first == m_last;               }

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
    using nonconst_partition = typename std::remove_const<Partition>::type;
    using container_type     = typename partition_traits<nonconst_partition>::container_type;
    using subrange_view_type = decltype (std::declval<partition_type> ().template subrange_view<0> ());
    using view_array         = std::array<subrange_view_type, N>;

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
      noexcept (noexcept (std::swap (std::declval<std::array<subrange_view_type, N>&> (),
                                     std::declval<std::array<subrange_view_type, N>&> ())))
    {
      std::swap (m_subrange_views, other.m_subrange_views);
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

#ifdef GCH_LIB_THREE_WAY_COMPARISON

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
    typename std::enable_if<(Index < partition_size<nonconst_partition>::value)>::type
    init_views (partition_type& p)
    {
      m_subrange_views[Index] = p.template subrange_view<Index> ();
      init_views<Index + 1> (p);
    }

    template <std::size_t Index>
    typename std::enable_if<(Index == partition_size<nonconst_partition>::value)>::type
    init_views (const partition_type&) { }

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

#ifdef GCH_HAS_VARIANT
#  define GCH_PARTITION_ITERATOR

  template <typename Partition>
  class partition_iterator
  {
    template <typename IndexSequence>
    struct find_variant_type;

    template <std::size_t ...Indices>
    struct find_variant_type<std::index_sequence<Indices...>>
    {
      using type = std::variant<std::reference_wrapper<partition_element_t<Indices, Partition>>...>;
    };

  public:
    using subrange_indices = std::make_index_sequence<Partition::size ()>;
    using variant_subrange = typename find_variant_type<subrange_indices>::type;

    using value_type        = variant_subrange;
    using reference         = variant_subrange;
    using pointer           = variant_subrange *;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

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
    struct overload_wrapper : Fs...
    {
      template <typename ...Ts,
                typename = std::enable_if_t<std::conjunction_v<
                  std::is_same<std::decay_t<Ts>, std::decay_t<Fs>>...>>>
      constexpr overload_wrapper (Ts&&... fs) noexcept
        : Fs (std::forward<Ts> (fs))...
      { }

      template <typename T>
      constexpr decltype (auto) call (T&& t) const
      {
        return operator() (std::forward<T> (t));
      }

      using Fs::operator()...;
    };

    template <typename Partition, typename Derived, std::size_t Idx>
    struct subrange_overload;

    template <typename Partition, template <typename...> class DerivedT,
      std::size_t Idx, typename ...Fs>
    struct subrange_overload<Partition, DerivedT<Partition, Fs...>, Idx>
    {
      using derived_type  = DerivedT<Partition, Fs...>;
      using subrange_type = partition_element_t<Idx, Partition>;
      using wrapper_type  = overload_wrapper<Fs...>;

      template <typename T,
        typename = std::enable_if_t<
          std::is_same_v<std::decay_t<T>, std::reference_wrapper<subrange_type>>>>
      constexpr decltype (auto) operator() (T&& r) const
      {
        return wrapper_cast (this)->call (r.get ());
      }

      static constexpr auto wrapper_cast (const subrange_overload *ptr)
      {
        return static_cast<const wrapper_type *> (static_cast<const derived_type *> (ptr));
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

  } // gch::detail

  template <typename Partition, typename ...Fs>
  struct partition_overloader
    : detail::overload_wrapper<Fs...>,
      detail::partition_overload_impl<Partition, partition_overloader<Partition, Fs...>,
                                      std::make_index_sequence<Partition::size ()>>
  {
    using detail::overload_wrapper<Fs...>::overload_wrapper;
    using detail::partition_overload_impl<Partition, partition_overloader,
                                          std::make_index_sequence<Partition::size ()>>::operator();
  };

  template <typename Partition, typename ...Fs,
            typename = std::enable_if_t<std::conjunction_v<
              std::negation<std::is_same<std::decay_t<Fs>, Partition>>...>>>
  constexpr auto partition_overload (Fs&&... fs)
  {
    return partition_overloader<Partition, Fs...> (std::forward<Fs> (fs)...);
  }

  template <typename Partition, typename ...Fs>
  constexpr auto partition_overload (const Partition&, Fs&&... fs)
  {
    return partition_overloader<Partition, Fs...> (std::forward<Fs> (fs)...);
  }

#endif

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  namespace detail::compare
  {

    static constexpr struct synth_three_way_functor
    {
      template <typename T, typename U>
      constexpr auto operator() (const T& lhs, const U& rhs) const
        noexcept (noexcept (lhs <=> rhs))
        requires std::three_way_comparable_with<T, U>
      {
        return lhs <=> rhs;
      }

      template <typename T, typename U>
      constexpr auto operator() (const T& lhs, const U& rhs) const
        requires (! std::three_way_comparable_with<T, U>)
      {
        return (lhs < rhs) ? std::weak_ordering::less
                           : (rhs < lhs) ? std::weak_ordering::greater
                                         : std::weak_ordering::equivalent;
      }
    } synth_three_way;

  } // namespace gch::detail::compare

#endif

} // namespace gch

namespace std
{
  template <typename Partition, std::size_t N>
  struct tuple_size<gch::partition_view<Partition, N>>
    : public std::integral_constant<std::size_t, N>
  { };

  template <std::size_t I, typename Partition, std::size_t N>
  struct tuple_element<I, gch::partition_view<Partition, N>>
  {
    using type = typename gch::partition_view<Partition, N>::subrange_view_type;
  };
}

#endif // PARTITION_HPP
