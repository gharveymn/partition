/** main.cpp
 * Short description here. 
 * 
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <partition.hpp>
#include <list_partition.hpp>

#include <algorithm>
#include <iostream>
#include <list>
#include <tuple>

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
  
  gch::dependent_subrange<std::list<int>>& get_phi (void) { return m_phi_range; }
  const gch::dependent_subrange<std::list<int>>& get_phi (void) const { return m_phi_range; }
  
  gch::dependent_subrange<std::list<int>>& get_body (void) { return m_body_range; }
  const gch::dependent_subrange<std::list<int>>& get_body (void) const { return m_body_range; }

private:
  std::list<int> m_data;
  std::list<int>::iterator m_pivot;
  gch::dependent_subrange<std::list<int>> m_phi_range;
  gch::dependent_subrange<std::list<int>> m_body_range;
  
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
  
  gch::dependent_subrange<std::list<int>>& get_phi (void) { return gch::get<0> (m_partition); }
  const gch::dependent_subrange<std::list<int>>& get_phi (void) const { return gch::get<0> (m_partition); }
  
  gch::dependent_subrange<std::list<int>>& get_body (void) { return gch::get<1> (m_partition); }
  const gch::dependent_subrange<std::list<int>>& get_body (void) const { return gch::get<1> (m_partition); }

private:
  std::list<int> m_data;
  std::list<int>::iterator m_pivot;
  gch::dependent_partition_view<std::list<int>, 2> m_partition;
  
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

template <std::size_t idx, typename T, std::size_t N, typename Container>
typename std::enable_if<(idx == N)>::type print_subrange (gch::list_partition<T, N, Container>& p)
{
  std::cout << std::endl;
}

template <std::size_t idx, typename T, std::size_t N, typename Container>
typename std::enable_if<(idx < N)>::type print_subrange (gch::list_partition<T, N, Container>& p)
{
  gch::list_partition_subrange<Container, N, idx>& r = gch::get<idx> (p);
  std::cout << "[ ";
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

template <typename T, std::size_t N, typename Container>
void print_partition (gch::list_partition<T, N, Container>& p)
{
  print_subrange<0> (p);
}

template <typename View>
void print_view (const View& v)
{
  for (auto&& s : v)
  {
    std::cout << "[ ";
    if (!s.empty ())
    {
      std::cout << s.front ();
      for (auto it = ++s.begin (); it != s.end (); ++it)
        std::cout << ", " << *it;
    }
    std::cout << " ]" << std::endl;
  }
  std::cout << std::endl;
}

void do_test_list_partition (void)
{
  gch::list_partition<int, 3> x;
  auto& r1 = gch::get<0> (x);
  auto& r2 = gch::get<1> (x);
  auto& r3 = gch::get<2> (x);
  
  r1.emplace_back (1);
  print_partition (x);
  
  r1.emplace_back (3);
  print_partition (x);
  
  r3.emplace_back (7);
  print_partition (x);
  
  r3.emplace_back (9);
  print_partition (x);
  
  r2.emplace_back (17);
  print_partition (x);
  
  auto pv = x.view ();
  print_view (pv);
  
  const auto& y = x;
  auto cpv = y.view ();
  print_view (cpv);
  
}

int main (void)
{
  auto s1 = do_test_subrange (test_subrange ());
  auto s2 = do_test_subrange (test_partition ());
  do_test_list_partition ();
  return 0;
}
