/** main.cpp
 * Short description here.
 *
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <gch/partition/partition.hpp>
#include <gch/partition/dependent_partition.hpp>
#include <gch/partition/list_partition.hpp>
#include <gch/partition/vector_partition.hpp>

#include <algorithm>
#include <iostream>
#include <list>
#include <tuple>
#include <string>
#include <iterator>
#include <chrono>

#if __cpp_concepts >= 201907L
#  ifndef GCH_CONCEPTS
#    define GCH_CONCEPTS
#  endif
#  if __has_include(<concepts>)
#    include <concepts>
#    if __cpp_lib_concepts >= 202002L
#      ifndef GCH_LIB_CONCEPTS
#        define GCH_LIB_CONCEPTS
#      endif
#    endif
#  endif
#endif

#ifdef GCH_LIB_CONCEPTS

template <typename T>
concept referenceable = std::is_same_v<T&, std::add_lvalue_reference_t<T>>;

template<class I>
concept LegacyIterator =
requires(I i) {
  {   *i } -> referenceable;
  {  ++i } -> std::same_as<I&>;
  { *i++ } -> referenceable;
} && std::copyable<I>;

template<class I>
concept LegacyInputIterator =
  LegacyIterator<I> &&
  std::equality_comparable<I> &&
  requires(I i)
  {
    typename std::iter_difference_t<I>;
    typename std::indirectly_readable_traits<I>::value_type;
    typename std::common_reference_t<std::iter_reference_t<I>&&,
                                     typename std::indirectly_readable_traits<I>::value_type&>;
    *i++;
    typename std::common_reference_t<decltype(*i++)&&,
                                     typename std::indirectly_readable_traits<I>::value_type&>;
    requires std::signed_integral<std::iter_difference_t<I>>;
  };

template<class I>
concept LegacyForwardIterator =
  LegacyInputIterator<I> &&
  std::constructible_from<I> &&
  std::is_lvalue_reference_v<std::iter_reference_t<I>> &&
  std::same_as<std::remove_cvref_t<std::iter_reference_t<I>>,
               typename std::indirectly_readable_traits<I>::value_type> &&
  requires(I i)
  {
    {  i++ } -> std::convertible_to<const I&>;
    { *i++ } -> std::same_as<std::iter_reference_t<I>>;
  };

template <typename T, typename X, typename A>
concept Erasable =
  std::same_as<typename X::value_type, T> &&
  std::same_as<typename X::allocator_type,
               typename std::allocator_traits<A>::template rebind_alloc<T>> &&
  requires (A m, T *p)
  {
    { std::allocator_traits<A>::destroy (m, p) };
  };

template <typename T, typename X, typename A>
concept MoveInsertable =
  std::same_as<typename X::value_type, T> &&
  std::same_as<typename X::allocator_type,
               typename std::allocator_traits<A>::template rebind_alloc<T>> &&
  requires (A m, T *p, T&& rv)
  {
    { std::allocator_traits<A>::construct (m, p, rv) };
  };

template <typename T, typename X, typename A>
concept CopyInsertable =
  std::same_as<typename X::value_type, T> &&
  std::same_as<typename X::allocator_type,
               typename std::allocator_traits<A>::template rebind_alloc<T>> &&
  MoveInsertable<T, X, A> &&
  requires (A m, T *p,       T v) { std::allocator_traits<A>::construct (m, p, v); } &&
  requires (A m, T *p, const T v) { std::allocator_traits<A>::construct (m, p, v); };

template <typename Iterator, typename Container>
concept ContainerIterator =
  LegacyForwardIterator<Iterator> &&
  std::same_as<Iterator, typename Container::iterator> ||
    std::same_as<Iterator, typename Container::const_iterator>;

template <typename C, typename T, typename A>
concept Container =
  requires (C a, C b, C&& rv)
  {
    typename C::value_type;
    typename C::reference;
    typename C::const_reference;
    typename C::iterator;
    typename C::const_iterator;
    typename C::difference_type;
    typename C::size_type;
    
    requires std::same_as<typename C::value_type, T>;
    requires std::same_as<typename C::reference, T&>;
    requires std::same_as<typename C::const_reference, const T&>;
  
    requires Erasable<T, C, A>;
    
    requires LegacyForwardIterator<typename C::iterator> &&
             std::convertible_to<typename C::iterator,
                                 typename C::const_iterator>;
    
    requires LegacyForwardIterator<typename C::const_iterator>;
    
    requires std::signed_integral<typename C::difference_type> &&
             std::same_as<typename C::difference_type,
               typename std::iterator_traits<typename C::iterator>::difference_type> &&
             std::same_as<typename C::difference_type,
               typename std::iterator_traits<typename C::const_iterator>::difference_type>;
  
    requires std::unsigned_integral<typename C::size_type> &&
             (std::numeric_limits<typename C::difference_type>::max () <=
               std::numeric_limits<typename C::size_type>::max ());
    
    requires std::default_initializable<C>;
    requires std::copy_constructible<C>;
    requires std::equality_comparable<C>;
    requires std::swappable<C>;
    
    requires CopyInsertable<T, C, A>;
    requires std::equality_comparable<T>;
    requires std::destructible<T>;
    requires std::signed_integral<typename C::difference_type>;
  
    { C ()          } -> std::same_as<C>;
    { C (a)         } -> std::same_as<C>;
    { C (rv)        } -> std::same_as<C>;
    { a = b         } -> std::same_as<C&>;
    { a = rv        } -> std::same_as<C&>;
    { a.~C ()       } -> std::same_as<void>;
    { a.begin ()    } -> ContainerIterator<C>;
    { a.end ()      } -> ContainerIterator<C>;
    { a.cbegin ()   } -> std::same_as<typename C::const_iterator>;
    { a.cend ()     } -> std::same_as<typename C::const_iterator>;
    { a == b        } -> std::convertible_to<bool>;
    { a != b        } -> std::convertible_to<bool>;
    { a.swap (b)    } -> std::same_as<void>;
    { swap (a, b)   } -> std::same_as<void>;
    { a.size ()     } -> std::same_as<typename C::size_type>;
    { a.max_size () } -> std::same_as<typename C::size_type>;
    { a.empty ()    } -> std::convertible_to<bool>;
  
    { const_cast<const C&> (a).begin () } -> std::same_as<typename C::const_iterator>;
    { const_cast<const C&> (a).end ()   } -> std::same_as<typename C::const_iterator>;
  };

static_assert (Container<std::list<int>, int, std::list<int>::allocator_type>);
static_assert (! Container<int, char, long>);

template <typename X, typename T, typename A>
concept AllocatorAwareContainer =
  requires (X a, X b, X t, X&& rv, A m)
  {
    typename X::allocator_type;
    requires std::same_as<typename X::allocator_type, A>;
    requires std::same_as<typename A::value_type, typename X::value_type>;
    
    requires std::default_initializable<A>;
    requires CopyInsertable<T, X, A>;
    requires std::is_copy_assignable_v<X>;
    
    { a.get_allocator ()            } -> std::same_as<A>;
    { X ()                          };
    { X (m)                         };
    { X (t, m)                      };
    { X (rv)                        };
    { X (rv, m)                     };
    { a = t                         } -> std::same_as<X&>;
    { a = const_cast<const X&&> (t) } -> std::same_as<X&>;
    { a = rv                        } -> std::same_as<X&>;
    { a.swap (b)                    } -> std::same_as<void>;
  };

static_assert (AllocatorAwareContainer<std::list<int>, int, std::list<int>::allocator_type>);
static_assert (! AllocatorAwareContainer<int, char, long>);

static_assert (Container<gch::list_partition_subrange<std::list<int>, 4, 1>, int, std::list<int>::allocator_type>);

#endif

namespace gch
{
  template class list_partition<std::string, 4>;
  template class list_partition_subrange<std::list<std::string>, 4, 0>;
  template class list_partition_subrange<std::list<std::string>, 4, 1>;
  template class list_partition_subrange<std::list<std::string>, 4, 2>;
  template class list_partition_subrange<std::list<std::string>, 4, 3>;
  template class list_partition_subrange<std::list<std::string>, 4, 4>;
  
  template class vector_partition<std::string, 4>;
  template class vector_partition_subrange<std::vector<std::string>, 4, 0>;
  template class vector_partition_subrange<std::vector<std::string>, 4, 1>;
  template class vector_partition_subrange<std::vector<std::string>, 4, 2>;
  template class vector_partition_subrange<std::vector<std::string>, 4, 3>;
  template class vector_partition_subrange<std::vector<std::string>, 4, 4>;
}

using namespace gch;

class test_subrange
{
public:
  
  using list   = std::list<int>;
  using iter   = typename list::iterator;
  using citer  = typename list::const_iterator;
  using riter  = typename list::reverse_iterator;
  using criter = typename list::const_reverse_iterator;
  using ref    = typename list::reference;
  using cref   = typename list::const_reference;
  
  [[nodiscard]] iter   begin   (void)       noexcept { return m_data.begin ();   }
  [[nodiscard]] citer  begin   (void) const noexcept { return m_data.begin ();   }
  [[nodiscard]] citer  cbegin  (void) const noexcept { return m_data.cbegin ();  }
  
  [[nodiscard]] iter   end     (void)       noexcept { return m_data.end ();     }
  [[nodiscard]] citer  end     (void) const noexcept { return m_data.end ();     }
  [[nodiscard]] citer  cend    (void) const noexcept { return m_data.cend ();    }
  
  [[nodiscard]] riter  rbegin  (void)       noexcept { return m_data.rbegin ();  }
  [[nodiscard]] criter rbegin  (void) const noexcept { return m_data.rbegin ();  }
  [[nodiscard]] criter crbegin (void) const noexcept { return m_data.crbegin (); }
  
  [[nodiscard]] riter  rend    (void)       noexcept { return m_data.rend ();    }
  [[nodiscard]] criter rend    (void) const noexcept { return m_data.rend ();    }
  [[nodiscard]] criter crend   (void) const noexcept { return m_data.crend ();   }
  
  [[nodiscard]] ref&   front   (void)       noexcept { return m_data.front ();   }
  [[nodiscard]] cref&  front   (void) const noexcept { return m_data.front ();   }
  
  [[nodiscard]] ref&   back    (void)       noexcept { return m_data.back ();    }
  [[nodiscard]] cref&  back    (void) const noexcept { return m_data.back ();    }
  
  [[nodiscard]] iter get_pivot (void) const noexcept { return m_pivot; }
  
  // test_subrange1 (void)
  //   : m_pivot (m_data.end ()),
  //     m_phi_range  ([this] (void) -> iter  { return begin (); },
  //                   [this] (void) -> iter  { return get_pivot (); }),
  //     m_body_range ([this] (void) -> iter  { return get_pivot (); },
  //                   [this] (void) -> iter  { return end (); })
  // { }
  
  test_subrange (void)
    : m_pivot (m_data.end ()),
      m_phi_range  ([this] (void) -> iter  { return begin (); },
                    [this] (void) -> iter  { return get_pivot (); }),
      m_body_range ([this] (void) -> iter  { return get_pivot (); },
                    [this] (void) -> iter  { return end (); })
  { }
  
  void emplace_body (int x)
  {
    m_data.emplace_back (x);
    if (m_pivot == end ())
      --m_pivot;
  }
  
  void emplace_phi (int x)
  {
    m_data.emplace (m_pivot, x);
  }
  
  void print_phi (void)
  {
    for (int i : get_phi ())
      std::cout << i << std::endl;
    std::cout << std::endl;
  }
  
  void print_body (void) const
  {
    for (int i : get_body ())
      std::cout << i << std::endl;
    std::cout << std::endl;
  }
  
  void rprint_phi (void)
  {
    std::for_each (get_phi ().rbegin (), get_phi ().rend (), [](int i) { std::cout << i << std::endl; });
    std::cout << std::endl;
  }
  
  void rprint_body (void) const
  {
    std::for_each (get_body ().rbegin (), get_body ().rend (), [](int i) { std::cout << i << std::endl; });
    std::cout << std::endl;
  }
  
  dependent_subrange<std::list<int>>& get_phi (void) { return m_phi_range; }
  const dependent_subrange<std::list<int>>& get_phi (void) const { return m_phi_range; }
  
  dependent_subrange<std::list<int>>& get_body (void) { return m_body_range; }
  const dependent_subrange<std::list<int>>& get_body (void) const { return m_body_range; }

private:
  std::list<int> m_data;
  std::list<int>::iterator m_pivot;
  dependent_subrange<std::list<int>> m_phi_range;
  dependent_subrange<std::list<int>> m_body_range;
  
};

class test_partition
{
public:
  
  using list   = std::list<int>;
  using iter   = typename list::iterator;
  using citer  = typename list::const_iterator;
  using riter  = typename list::reverse_iterator;
  using criter = typename list::const_reverse_iterator;
  using ref  = typename list::reference;
  using cref = typename list::const_reference;
  
  [[nodiscard]] iter   begin   (void)       noexcept { return m_data.begin ();   }
  [[nodiscard]] citer  begin   (void) const noexcept { return m_data.begin ();   }
  [[nodiscard]] citer  cbegin  (void) const noexcept { return m_data.cbegin ();  }
  
  [[nodiscard]] iter   end     (void)       noexcept { return m_data.end ();     }
  [[nodiscard]] citer  end     (void) const noexcept { return m_data.end ();     }
  [[nodiscard]] citer  cend    (void) const noexcept { return m_data.cend ();    }
  
  [[nodiscard]] riter  rbegin  (void)       noexcept { return m_data.rbegin ();  }
  [[nodiscard]] criter rbegin  (void) const noexcept { return m_data.rbegin ();  }
  [[nodiscard]] criter crbegin (void) const noexcept { return m_data.crbegin (); }
  
  [[nodiscard]] riter  rend    (void)       noexcept { return m_data.rend ();    }
  [[nodiscard]] criter rend    (void) const noexcept { return m_data.rend ();    }
  [[nodiscard]] criter crend   (void) const noexcept { return m_data.crend ();   }
  
  [[nodiscard]] ref&   front   (void)       noexcept { return m_data.front ();   }
  [[nodiscard]] cref&  front   (void) const noexcept { return m_data.front ();   }
  
  [[nodiscard]] ref&   back    (void)       noexcept { return m_data.back ();    }
  [[nodiscard]] cref&  back    (void) const noexcept { return m_data.back ();    }
  
  [[nodiscard]] iter get_pivot (void) const noexcept { return m_pivot; }
  
  test_partition (void)
    : m_pivot (m_data.end ()),
      m_partition (m_data, [this] (void) -> iter  { return get_pivot (); })
  { }
  
  explicit test_partition (int)
    : m_pivot (m_data.end ()),
      m_partition ([this] (void) -> iter  { return begin (); },
                   [this] (void) -> iter  { return get_pivot (); },
                   [this] (void) -> iter  { return end (); })
  { }
  
  void emplace_body (int x)
  {
    m_data.emplace_back (x);
    if (m_pivot == end ())
      --m_pivot;
  }
  
  void emplace_phi (int x)
  {
    m_data.emplace (m_pivot, x);
  }
  
  void print_phi (void)
  {
    for (int i : get_phi ())
      std::cout << i << std::endl;
    std::cout << std::endl;
  }
  
  void print_body (void) const
  {
    for (int i : get_body ())
      std::cout << i << std::endl;
    std::cout << std::endl;
  }
  
  void rprint_phi (void)
  {
    std::for_each (get_phi ().rbegin (), get_phi ().rend (), [](int i) { std::cout << i << std::endl; });
    std::cout << std::endl;
  }
  
  void rprint_body (void) const
  {
    std::for_each (get_body ().rbegin (), get_body ().rend (), [](int i) { std::cout << i << std::endl; });
    std::cout << std::endl;
  }
  
  dependent_subrange<std::list<int>>& get_phi (void) { return get<0> (m_partition); }
  const dependent_subrange<std::list<int>>& get_phi (void) const { return get<0> (m_partition); }
  
  dependent_subrange<std::list<int>>& get_body (void) { return get<1> (m_partition); }
  const dependent_subrange<std::list<int>>& get_body (void) const { return get<1> (m_partition); }

private:
  std::list<int> m_data;
  std::list<int>::iterator m_pivot;
  dependent_partition_view<std::list<int>, 2> m_partition;
  
};

template <typename S>
S do_test_subrange (S&& s)
{
  for (int i : s)
    std::cout << i << std::endl;
  std::cout << std::endl;
  s.emplace_body (7);
  s.print_body ();
  
  s.emplace_body (8);
  s.print_body ();
  
  s.emplace_phi (3);
  s.print_phi ();
  
  s.rprint_phi ();
  s.rprint_body ();
  
  for (int i : s)
    std::cout << i << std::endl;
  std::cout << std::endl;
  
  std::cout << "num phi: " << s.get_phi ().size () << std::endl;
  return s;
}

void do_test_partition_view (void)
{
  test_partition p1;
  test_partition p2 (0);
}

template <std::size_t idx, typename T, std::size_t N, typename Container,
          template <typename, std::size_t, typename> class PartitionT>
typename std::enable_if<(idx == N)>::type print_subrange (PartitionT<T, N, Container>&)
{
  std::cout << std::endl;
}

template <std::size_t idx, typename T, std::size_t N, typename Container,
          template <typename, std::size_t, typename> class PartitionT>
typename std::enable_if<(idx < N)>::type print_subrange (PartitionT<T, N, Container>& p)
{
  auto& r = get_subrange<idx> (p);
  std::cout << "  [ ";
  if (! r.empty ())
  {
    std::cout << r.front ();
    std::for_each (++r.begin (), r.end (),
                   [] (int x)
                   {
                     std::cout << ", " << x;
                   });
  }
  std::cout << " ]" << std::endl;
  print_subrange<idx + 1, T, N, Container> (p);
}

template <typename Partition>
void print_partition (Partition& p)
{
  std::cout << "{" << std::endl;
  print_subrange<0> (p);
  std::cout << "}" << std::endl;
}

template <typename View>
void print_subrange_view (const View& v)
{
  std::cout << "  [ ";
  if (! v.empty ())
  {
    std::cout << v.front ();
    for (auto it = ++v.begin (); it != v.end (); ++it)
      std::cout << ", " << *it;
  }
  std::cout << " ]" << std::endl;
}

template <typename View>
void print_partition_view (const View& v)
{
  std::cout << "{" << std::endl;
  for (auto&& s : v)
    print_subrange_view (s);
  std::cout << "}" << std::endl;
}

template <template <typename, std::size_t, typename> class PartitionT, typename C>
void do_test_partition (void)
{
  PartitionT<int, 3, C> p1 { };
  auto& r1 = get_subrange<0> (p1);
  auto& r2 = get_subrange<1> (p1);
  auto& r3 = get<2> (p1);
  
  r1.emplace_back (1);
  print_partition (p1);
  
  r1.emplace_back (3);
  print_partition (p1);
  
  r3.emplace_back (7);
  print_partition (p1);
  
  r3.emplace_back (9);
  print_partition (p1);
  
  r2.emplace_back (17);
  print_partition (p1);
  
  auto& r22 = next_subrange (r1);
  std::cout << "subrange: ";
  print_subrange_view (r22.view ());
  
  auto& parent_part = get_partition (r1);
  
  auto& r11 = prev_subrange (r2);
  
  PartitionT<int, 5, C> p2;
  get_subrange<3> (p2).emplace_back (70);
  get_subrange<3> (p2).emplace_back (12);
  get_subrange<3> (p2).emplace_back (97);
  get_subrange<3> (p2).emplace_back (82);
  
  auto p3 = partition_cat (p1, p2);
  auto p4 = partition_cat (p1, partition_cat (p1, p2), p3);
  
  print_partition_view (p3.partition_view ());
  print_partition_view (p4.partition_view ());
  
  PartitionT<int, 3, C> p6 (std::allocator<int> { });
  
  auto pv = p1.partition_view ();
  print_partition_view (pv);
  
  const auto& y = p1;
  auto cpv = y.partition_view ();
  print_partition_view (cpv);
  
  std::cout << std::endl;

#ifdef GCH_PARTITION_ITERATOR
  
  partition_iterator<decltype (p1)> it = p1.begin ();
  partition_iterator<const decltype (p1)> cit = it;
  for (const auto& e : p1)
  {
    std::visit ([] (auto&& arg) { std::cout << arg.get ().size (); }, e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (decltype (p1)::overload ([] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (p1.overload ([] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (partition_overload<decltype (p1)> ([] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (partition_overload (p1, [] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;

#endif

}

void do_test_list_partition (void)
{
  list_partition<int, 3> p1;
  auto& r1 = get_subrange<0> (p1);
  auto& r2 = get_subrange<1> (p1);
  auto& r3 = get_subrange<2> (p1);
  
  r1.emplace_back (1);
  print_partition (p1);
  
  r1.emplace_back (3);
  print_partition (p1);
  
  r3.emplace_back (7);
  print_partition (p1);
  
  r3.emplace_back (9);
  print_partition (p1);
  
  r2.emplace_back (17);
  print_partition (p1);
  
  auto& r22 = next_subrange (r1);
  std::cout << "subrange: ";
  print_subrange_view (r22.view ());
  
  auto& parent_part = get_partition (r1);
  static_cast<void> (parent_part);
  
  auto& r11 = prev_subrange (r2);
  static_cast<void> (r11);
  
  list_partition<int, 5> p2;
  get_subrange<3> (p2).emplace_back (70);
  get_subrange<3> (p2).emplace_back (12);
  get_subrange<3> (p2).emplace_back (97);
  get_subrange<3> (p2).emplace_back (82);
  
  auto p3 = partition_cat (p1, p2);
  auto p4 = partition_cat (p1, partition_cat (p1, p2), p3);
  
  print_partition_view (p3.partition_view ());
  print_partition_view (p4.partition_view ());
  
  list_partition<int, 3> p6 (std::list<int>::allocator_type { });
  
  auto pv = p1.partition_view ();
  print_partition_view (pv);
  
  const auto& y = p1;
  auto cpv = y.partition_view ();
  print_partition_view (cpv);
  
  std::cout << "advance begin: \n";
  p4.advance_begin<2> (1);
  print_partition_view (p4.partition_view ());
  
  std::cout << "advance end: \n";
  p4.advance_end<9> (3);
  print_partition_view (p4.partition_view ());
  
  try
  {
    p4.advance_end<9> (100);
  }
  catch (...)
  {
    std::cout << "past end exception successfully caught" << std::endl;
  }
  
  try
  {
    p4.advance_begin<9> (-100);
  }
  catch (...)
  {
    std::cout << "past begin exception successfully caught" << std::endl;
  }
  
  std::cout << std::endl;

#ifdef GCH_PARTITION_ITERATOR
  
  partition_iterator<decltype (p1)> it = p1.begin ();
  partition_iterator<const decltype (p1)> cit = it;
  static_cast<void> (cit);
  for (const auto& e : p1)
  {
    std::visit ([] (auto&& arg) { std::cout << arg.get ().size (); }, e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (decltype (p1)::overload ([] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (p1.overload ([] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (partition_overload<decltype (p1)> ([] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (partition_overload (p1, [] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
#endif

}

void do_test_vector_partition (void)
{
  vector_partition<int, 3> p1;
  auto& r1 = get_subrange<0> (p1);
  auto& r2 = get_subrange<1> (p1);
  auto& r3 = get_subrange<2> (p1);
  
  r1.emplace_back (1);
  print_partition (p1);
  
  r1.emplace_back (3);
  print_partition (p1);
  
  r3.emplace_back (7);
  print_partition (p1);
  
  r3.emplace_back (9);
  print_partition (p1);
  
  r2.emplace_back (17);
  print_partition (p1);
  
  auto& r22 = next_subrange (r1);
  std::cout << "subrange: ";
  print_subrange_view (r22.view ());
  
  auto& parent_part = get_partition (r1);
  static_cast<void> (parent_part);
  
  auto& r11 = prev_subrange (r2);
  static_cast<void> (r11);
  
  vector_partition<int, 5> p2;
  get_subrange<3> (p2).emplace_back (70);
  get_subrange<3> (p2).emplace_back (12);
  get_subrange<3> (p2).emplace_back (97);
  get_subrange<3> (p2).emplace_back (82);
  
  auto p3 = partition_cat (p1, p2);
  auto p4 = partition_cat (p1, partition_cat (p1, p2), p3);
  
  print_partition_view (p3.partition_view ());
  print_partition_view (p4.partition_view ());
  
  vector_partition<int, 3> p6 (std::vector<int>::allocator_type { });
  
  auto pv = p1.partition_view ();
  print_partition_view (pv);
  
  const auto& y = p1;
  auto cpv = y.partition_view ();
  print_partition_view (cpv);
  
  std::cout << "advance begin: \n";
  p4.advance_begin<2> (1);
  print_partition_view (p4.partition_view ());
  
  std::cout << "advance end: \n";
  p4.advance_end<9> (3);
  print_partition_view (p4.partition_view ());
  
  try
  {
    p4.advance_end<9> (100);
  }
  catch (...)
  {
    std::cout << "past end exception successfully caught" << std::endl;
  }
  
  try
  {
    p4.advance_begin<9> (-100);
  }
  catch (...)
  {
    std::cout << "past begin exception successfully caught" << std::endl;
  }
  
  std::cout << std::endl;

#ifdef GCH_PARTITION_ITERATOR
  
  partition_iterator<decltype (p1)> it = p1.begin ();
  partition_iterator<const decltype (p1)> cit = it;
  static_cast<void> (cit);
  for (const auto& e : p1)
  {
    std::visit ([] (auto&& arg) { std::cout << arg.get ().size (); }, e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (decltype (p1)::overload ([] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (p1.overload ([] (auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (partition_overload<decltype (p1)> ([](auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;
  
  for (const auto& e : p1)
  {
    std::visit (partition_overload (p1, [](auto&& arg) { std::cout << arg.size (); }), e);
  }
  std::cout << std::endl;

#endif

}

enum class key
{
  first  = 0,
  second = 1,
  third  = 2,
};

enum class key1 : int
{
  first  = 0,
  second = 1,
  third  = 2,
};

enum key2
{
  first  = 0,
  second = 1,
  third  = 2,
};

enum key3 : int
{
  first1  = 0,
  second1 = 1,
  third1  = 2,
};

enum class key4
{
  first,
  second,
  third,
  forth,
  fifth,
};

enum key5
{
  first2,
  second2,
  third2,
  forth2,
  fifth2,
};

void do_test_enum_access (void)
{
#ifdef GCH_TEMPLATE_AUTO
  
  list_partition<int, 3> x;
  get_subrange<key::first> (x).emplace_back (5);
  get_subrange<key::first> (x).emplace_back (6);
  get_subrange<key::third> (x).emplace_back (17);
  get_subrange<key::second> (x).emplace_back (3);
  get_subrange<key::second> (x).emplace_back (3);

  std::for_each (x.data_begin (), x.data_end (), [](int e) { std::cout << e << std::endl; });

  auto print = [](auto&& s) { std::for_each (s.begin (), s.end (),
                                             [](int e)
                                             {
                                               std::cout << e << std::endl;
                                             });
                              std::cout << std::endl;
                            };

  print (get_subrange<'\0'> (x));
  print (get_subrange<key1::first> (x));
  print (get_subrange<first> (x));
  print (get_subrange<first1> (x));
  print (get_subrange<key4::first> (x));
  print (get_subrange<first2> (x));
  // print (get_subrange<key4::forth> (x));
  // print (get_subrange<forth2> (x));
  // print (get_subrange<key4::fifth> (x));
  // print (get_subrange<fifth2> (x));
  
#else

  list_partition<int, 3> x;
  get_subrange<key, key::first> (x).emplace_back (5);
  get_subrange<key, key::first> (x).emplace_back (6);
  get_subrange<key, key::third> (x).emplace_back (17);
  get_subrange<key, key::second> (x).emplace_back (3);
  get_subrange<key, key::second> (x).emplace_back (3);

  std::for_each (x.data_begin (), x.data_end (), [](int e) { std::cout << e << std::endl; });

  auto print = [](partition_element_t<0, decltype (x)>& s) { std::for_each (s.begin (), s.end (),
                                             [](int e)
                                             {
                                               std::cout << e << std::endl;
                                             });
                              std::cout << std::endl;
                            };
  
  print (get_subrange<char, '\0'> (x));
  print (get_subrange<key1, key1::first> (x));
  print (get_subrange<key2, first> (x));
  print (get_subrange<key3, first1> (x));
  print (get_subrange<key4, key4::first> (x));
  print (get_subrange<key5, first2> (x));
  // print (get_subrange<key4::forth> (x));
  // print (get_subrange<forth2> (x));
  // print (get_subrange<key4::fifth> (x));
  // print (get_subrange<fifth2> (x));

#endif
}

const int repeat = 1;

int main (void)
{
  // auto s1 = do_test_subrange (test_subrange ());
  // auto s2 = do_test_subrange (test_partition ());
  
  using clock = std::chrono::high_resolution_clock;
  auto ts = clock::now ();
  for (int i = 0; i < repeat; ++i)
  {
    do_test_list_partition ();
  }
  auto tl = clock::now ();
  
  for (int i = 0; i < repeat; ++i)
  {
    do_test_vector_partition ();
  }
  auto tv = clock::now ();
  
  std::cout << "list took:   " << std::chrono::duration_cast<std::chrono::nanoseconds> (tl - ts).count () << " nanoseconds." << std::endl;
  std::cout << "vector took: " << std::chrono::duration_cast<std::chrono::nanoseconds> (tv - ts).count () << " nanoseconds." << std::endl;

#ifdef GCH_TEMPLATE_AUTO
  do_test_enum_access ();
#endif
  return 0;
}
