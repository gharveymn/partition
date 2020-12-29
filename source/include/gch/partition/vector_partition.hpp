/** vector_partition.hpp
 * Short description here.
 *
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef PARTITION_VECTOR_PARTITION_HPP
#define PARTITION_VECTOR_PARTITION_HPP

#include "partition.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

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

#ifndef GCH_CPP17_NOEXCEPT
#  if __cpp_constexpr >= 201603L
#    define GCH_CPP17_NOEXCEPT noexcept
#  else
#    define GCH_CPP17_NOEXCEPT
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

namespace gch
{

  template <typename T, std::size_t N, typename Container = std::vector<T>>
  class vector_partition;

  template <typename T, std::size_t N, typename Container>
  class partition_subrange<vector_partition<T, N, Container>, 0>
    : public partition_subrange<vector_partition<T, N, Container>, 1>
  {
    template <typename, std::size_t, typename>
    friend class partition_subrange;

  public:
    using partition_type = vector_partition<T, N, Container>;
    using subrange_type  = partition_subrange<partition_type, 0>;
//  using prev_type      = void;
    using next_type      = partition_subrange<partition_type, 1>;

    using container_type         = Container;
    using iterator               = typename Container::iterator;
    using const_iterator         = typename Container::const_iterator;
    using reverse_iterator       = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;
    using reference              = typename Container::reference;
    using const_reference        = typename Container::const_reference;
    using size_type              = typename Container::size_type;
    using difference_type        = typename Container::difference_type;
    using value_type             = typename Container::value_type;
    using allocator_type         = typename Container::allocator_type;

  private:
    using iter    = iterator;
    using citer   = const_iterator;
    using riter   = reverse_iterator;
    using criter  = const_reverse_iterator;
    using ref     = reference;
    using cref    = const_reference;
    using size_t  = size_type;
    using diff_t  = difference_type;
    using value_t = value_type;
    using alloc_t = allocator_type;

  protected:
    using next_type::m_container;

  public:
    partition_subrange            (void)                          = default;
    partition_subrange            (const partition_subrange&)     = default;
    partition_subrange            (partition_subrange&&) noexcept = default;
//  partition_subrange& operator= (const partition_subrange&)     = impl;
//  partition_subrange& operator= (partition_subrange&&) noexcept = impl;
    ~partition_subrange           (void)                          = default;

    partition_subrange& operator= (const partition_subrange& other) noexcept
    {
      if (&other != this)
        assign (other.begin (), other.end ());
      return *this;
    }

    partition_subrange& operator= (partition_subrange&& other) noexcept
    {
      assign (std::make_move_iterator (other.begin ()), std::make_move_iterator (other.end ()));
      return *this;
    }

  protected:
    template <std::size_t M, typename ...Subranges>
    constexpr partition_subrange (const partition_subrange<vector_partition<T, M, Container>, 0>& other,
                                       Subranges&&... subranges)
      : next_type (0, next_subrange (other), std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, typename ...Subranges>
    constexpr partition_subrange (partition_subrange<vector_partition<T, M, Container>, 0>&& other,
                                       Subranges&&... subranges)
      : next_type (0, next_subrange (other), std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, typename ...Subranges>
    constexpr partition_subrange (vector_partition<T, M, Container>&& partition,
                                       Subranges&&... subranges)
      : partition_subrange (get_subrange<0> (partition),
                                   std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, typename ...Subranges>
    constexpr partition_subrange (const vector_partition<T, M, Container>& partition,
                                       Subranges&&... subranges)
      : partition_subrange (get_subrange<0> (partition),
                                   std::forward<Subranges> (subranges)...)
    { }

    constexpr explicit partition_subrange (const alloc_t& alloc)
      : next_type (alloc)
    { }

  public:
    explicit partition_subrange (iter) { }

    void assign (size_t count, const value_t& val)
    {
      iter it = begin ();
      for (; it != end () && count != 0; ++it, --count)
        *it = val;

      if (count > 0)
        insert (end (), count, val);
      else
        erase (it, end ());
    }

    template <typename It>
    void assign (It first, It last)
    {
      iter curr = begin ();
      for (; curr != end () && first != last; ++curr, (void)++first)
        *curr = *first;

      if (first == last)
        erase(curr, end ());
      else
        insert(end (), first, last);
    }

    void assign (std::initializer_list<value_t> ilist)
    {
      assign (ilist.begin (), ilist.end ());
    }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR iter  begin  (void)       noexcept { return m_container.begin ();   }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer begin  (void) const noexcept { return m_container.cbegin ();  }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer cbegin (void) const noexcept { return m_container.cbegin ();  }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR iter   end     (void)       noexcept { return next_type::begin ();  }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer  end     (void) const noexcept { return next_type::cbegin (); }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer  cend    (void) const noexcept { return next_type::cbegin (); }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR riter  rbegin  (void)       noexcept { return next_type::rend ();   }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter rbegin  (void) const noexcept { return next_type::crend ();  }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter crbegin (void) const noexcept { return next_type::crend ();  }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR riter  rend    (void)       noexcept { return m_container.rend ();  }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter rend    (void) const noexcept { return m_container.crend (); }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter crend   (void) const noexcept { return m_container.crend (); }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR ref&   front   (void)       noexcept { return *begin ();            }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR cref&  front   (void) const noexcept { return *begin ();            }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR ref&   back    (void)       noexcept { return *(--end ());          }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR cref&  back    (void) const noexcept { return *(--cend ());         }

    GCH_NODISCARD GCH_CPP17_CONSTEXPR std::size_t size (void) const noexcept
    {
      return std::distance (cbegin (), cend ());
    }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR bool empty (void) const noexcept
    {
      return cbegin () == cend ();
    }

    void clear (void) noexcept
    {
      erase (cbegin(), cend ());
    }

    iter insert (const citer pos, const value_t& lv)
    {
      iter ret = m_container.insert (pos, lv);
      modify_offsets (1);
      return ret;
    }

    iter insert (const citer pos, value_t&& rv)
    {
      iter ret = m_container.insert (pos, std::move (rv));
      modify_offsets (1);
      return ret;
    }

    iter insert (const citer pos, size_t count, const value_t& val)
    {
      iter ret = m_container.insert (pos, count, val);
      modify_offsets (count);
      return ret;
    }

    template <typename Iterator>
    iter insert (const citer pos, Iterator first, Iterator last)
    {
      iter ret = m_container.insert (pos, first, last);
      modify_offsets (std::distance (first, last));
      return ret;
    }

    iter insert (const citer pos, std::initializer_list<value_t> ilist)
    {
      iter ret = m_container.insert (pos, ilist);
      modify_offsets (ilist.size ());
      return ret;
    }

    template <typename ...Args>
    iter emplace (const citer pos, Args&&... args)
    {
      iter ret = m_container.emplace (pos, std::forward<Args> (args)...);
      modify_offsets (1);
      return ret;
    }

    iter erase (const citer pos)
    {
      iter ret = m_container.erase (pos);
      modify_offsets (-1);
      return ret;
    }

    iter erase (const citer first, const citer last)
    {
      diff_t change = std::distance (last, first); // < 0
      iter ret = m_container.erase (first, last);
      modify_offsets (change);
      return ret;
    }

    void push_back (const value_t& val)
    {
      insert (cend (), val);
    }

    void push_back (value_t&& val)
    {
      insert (cend (), std::move (val));
    }

    template <typename ...Args>
    ref emplace_back (Args&&... args)
    {
      return *emplace (cend (), std::forward<Args> (args)...);
    }

    void pop_back (void)
    {
      erase (--cend ());
    }

    void push_front (const value_t& val)
    {
      insert (cbegin (), val);
    }

    void push_front (value_t&& val)
    {
      insert (cbegin (), std::move (val));
    }

    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      return *emplace (cbegin (), std::forward<Args> (args)...);
    }

    void resize (size_t count)
    {
      resize (count, { });
    }

    void resize (size_t count, const value_t& val)
    {
      if (count > size ())
        insert (cend (), count - size (), val);
      else if (count < size ())
        erase (std::next (cbegin (), count), cend ());
    }

    template <std::size_t M, std::size_t J>
    void swap (partition_subrange<vector_partition<T, M, Container>, J>& other)
    {
      if (&other == this)
        return;

      container_type tmp (std::make_move_iterator (begin ()), std::make_move_iterator (end ()));
      assign (std::make_move_iterator (other.begin ()), std::make_move_iterator (other.end ()));
      other.assign (std::make_move_iterator (tmp.begin ()), std::make_move_iterator (tmp.end ()));
    }

    subrange_view<iter> view (void)
    {
      return { begin (), end () };
    }

    subrange_view<citer> view (void) const
    {
      return { begin (), end () };
    }

    iter advance_begin (diff_t) = delete;

    iter advance_end (diff_t change)
    {
      return next_subrange (*this).advance_begin (change);
    }

  protected:
    void partition_swap (partition_subrange& other)
      noexcept (noexcept (std::declval<next_type&> ().partition_swap (other)))
    {
      next_type::partition_swap (other);
    }

    constexpr size_t get_offset (void) const noexcept { return 0; }

  private:
    void modify_offsets (diff_t change)
    {
      if (change == 0)
        return;

      next_subrange (*this).add_to_offset (change);
    }
  };

  template <typename T, std::size_t N, typename Container, std::size_t Index>
  class partition_subrange<vector_partition<T, N, Container>, Index,
                                typename std::enable_if<(0 < Index) && (Index < N)>::type>
    : public partition_subrange<vector_partition<T, N, Container>, Index + 1>
  {
    template <typename, std::size_t, typename>
    friend class partition_subrange;

  public:
    using partition_type = vector_partition<T, N, Container>;
    using subrange_type  = partition_subrange<partition_type, Index>;
    using prev_type      = partition_subrange<partition_type, Index - 1>;
    using next_type      = partition_subrange<partition_type, Index + 1>;

    using container_type         = Container;
    using iterator               = typename Container::iterator;
    using const_iterator         = typename Container::const_iterator;
    using reverse_iterator       = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;
    using reference              = typename Container::reference;
    using const_reference        = typename Container::const_reference;
    using size_type              = typename Container::size_type;
    using difference_type        = typename Container::difference_type;
    using value_type             = typename Container::value_type;
    using allocator_type         = typename Container::allocator_type;

  private:
    using iter    = iterator;
    using citer   = const_iterator;
    using riter   = reverse_iterator;
    using criter  = const_reverse_iterator;
    using ref     = reference;
    using cref    = const_reference;
    using size_t  = size_type;
    using diff_t  = difference_type;
    using value_t = value_type;
    using alloc_t = allocator_type;

  protected:
    using next_type::m_container;

  public:
    partition_subrange            (void)                                 = default;
    partition_subrange            (const partition_subrange&)     = default;
    partition_subrange            (partition_subrange&&) noexcept = default;
//  partition_subrange& operator= (const partition_subrange&)     = impl;
//  partition_subrange& operator= (partition_subrange&&) noexcept = impl;
    ~partition_subrange           (void)                                 = default;

  protected:
    template <std::size_t M, std::size_t J, typename ...Subranges,
      typename std::enable_if<(J < M)>::type * = nullptr>
    partition_subrange (size_t accum,
                        const partition_subrange<vector_partition<T, M, Container>, J>& other,
                        Subranges&&... subranges)
      : next_type (accum, next_subrange (other), std::forward<Subranges> (subranges)...),
        m_offset (other.m_offset + accum)
    { }

    template <std::size_t M, typename ...Subranges>
    partition_subrange (size_t accum,
                        const partition_subrange<vector_partition<T, M, Container>, M>& other,
                        Subranges&&... subranges)
      : partition_subrange (accum + other.m_container.size (),
                                   std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, std::size_t J, typename ...Subranges,
      typename std::enable_if<(J < M)>::type * = nullptr>
    partition_subrange (size_t accum,
                        partition_subrange<vector_partition<T, M, Container>, J>&& other,
                        Subranges&&... subranges)
      : next_type (accum, next_subrange (other), std::forward<Subranges> (subranges)...),
        m_offset (other.m_offset + accum)
    { }

    template <std::size_t M, typename ...Subranges>
    partition_subrange (size_t accum,
                        partition_subrange<vector_partition<T, M, Container>, M>&& other,
                        Subranges&&... subranges)
      : partition_subrange (accum + other.m_container.size (),
                                   std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, typename ...Subranges>
    constexpr partition_subrange (size_t accum, vector_partition<T, M, Container>&& partition,
                                  Subranges&&... subranges)
      : partition_subrange (accum, get_subrange<0> (partition),
                                   std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, typename ...Subranges>
    constexpr partition_subrange (size_t accum, const vector_partition<T, M, Container>& partition,
                                  Subranges&&... subranges)
      : partition_subrange (accum, get_subrange<0> (partition),
                                   std::forward<Subranges> (subranges)...)
    { }

    constexpr explicit partition_subrange (const alloc_t& alloc)
      : next_type (alloc)
    { }

  public:
    partition_subrange& operator= (const partition_subrange& other) noexcept
    {
      if (&other != this)
        assign (other.begin (), other.end ());
      return *this;
    }

    partition_subrange& operator= (partition_subrange&& other) noexcept
    {
      assign (std::make_move_iterator (other.begin ()), std::make_move_iterator (other.end ()));
      return *this;
    }

    void assign (size_t count, const value_t& val)
    {
      iter it = begin ();
      for (; it != end () && count != 0; ++it, --count)
        *it = val;

      if (count > 0)
        insert (end (), count, val);
      else
        erase (it, end ());
    }

    template <typename It>
    void assign (It first, It last)
    {
      iter curr = begin ();
      for (; curr != end () && first != last; ++curr, (void)++first)
        *curr = *first;

      if (first == last)
        erase(curr, end ());
      else
        insert(end (), first, last);
    }

    void assign (std::initializer_list<value_t> ilist)
    {
      assign (ilist.begin (), ilist.end ());
    }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR iter  begin  (void)       noexcept { return std::next (m_container.begin (), m_offset);   }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer begin  (void) const noexcept { return std::next (m_container.cbegin (), m_offset);  }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer cbegin (void) const noexcept { return std::next (m_container.cbegin (), m_offset);  }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR iter   end     (void)       noexcept { return next_type::begin ();                        }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer  end     (void) const noexcept { return next_type::cbegin ();                       }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer  cend    (void) const noexcept { return next_type::cbegin ();                       }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR riter  rbegin  (void)       noexcept { return next_type::rend ();                         }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter rbegin  (void) const noexcept { return next_type::crend ();                        }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter crbegin (void) const noexcept { return next_type::crend ();                        }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR riter  rend    (void)       noexcept { return std::prev (m_container.rend (), m_offset);  }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter rend    (void) const noexcept { return std::prev (m_container.crend (), m_offset); }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter crend   (void) const noexcept { return std::prev (m_container.crend (), m_offset); }

    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   front   (void)       noexcept { return *begin ();                                  }
    GCH_NODISCARD constexpr           cref&  front   (void) const noexcept { return *begin ();                                  }

    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   back    (void)       noexcept { return *(--end ());                                }
    GCH_NODISCARD constexpr           cref&  back    (void) const noexcept { return *(--cend ());                               }

    GCH_NODISCARD GCH_CPP17_CONSTEXPR std::size_t size (void) const noexcept
    {
      return std::distance (cbegin (), cend ());
    }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR bool empty (void) const noexcept
    {
      return cbegin () == cend ();
    }

    void clear (void) noexcept
    {
      erase (cbegin(), cend ());
    }

    iter insert (const citer pos, const value_t& lv)
    {
      iter ret = m_container.insert (pos, lv);
      modify_offsets (1);
      return ret;
    }

    iter insert (const citer pos, value_t&& rv)
    {
      iter ret = m_container.insert (pos, std::move (rv));
      modify_offsets (1);
      return ret;
    }

    iter insert (const citer pos, size_t count, const value_t& val)
    {
      iter ret = m_container.insert (pos, count, val);
      modify_offsets (count);
      return ret;
    }

    template <typename Iterator>
    iter insert (const citer pos, Iterator first, Iterator last)
    {
      iter ret = m_container.insert (pos, first, last);
      modify_offsets (std::distance (first, last));
      return ret;
    }

    iter insert (const citer pos, std::initializer_list<value_t> ilist)
    {
      iter ret = m_container.insert (pos, ilist);
      modify_offsets (ilist.size ());
      return ret;
    }

    template <typename ...Args>
    iter emplace (const citer pos, Args&&... args)
    {
      iter ret = m_container.emplace (pos, std::forward<Args> (args)...);
      modify_offsets (1);
      return ret;
    }

    iter erase (const citer pos)
    {
      iter ret = m_container.erase (pos);
      modify_offsets (-1);
      return ret;
    }

    iter erase (const citer first, const citer last)
    {
      diff_t change = std::distance (last, first); // < 0
      iter ret = m_container.erase (first, last);
      modify_offsets (change);
      return ret;
    }

    void push_back (const value_t& val)
    {
      insert (cend (), val);
    }

    void push_back (value_t&& val)
    {
      insert (cend (), std::move (val));
    }

    template <typename ...Args>
    ref emplace_back (Args&&... args)
    {
      return *emplace (cend (), std::forward<Args> (args)...);
    }

    void pop_back (void)
    {
      erase (--cend ());
    }

    void push_front (const value_t& val)
    {
      insert (cbegin (), val);
    }

    void push_front (value_t&& val)
    {
      insert (cbegin (), std::move (val));
    }

    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      return *emplace (cbegin (), std::forward<Args> (args)...);
    }

    void resize (size_t count)
    {
      resize (count, { });
    }

    void resize (size_t count, const value_t& val)
    {
      if (count > size ())
        insert (cend (), count - size (), val);
      else if (count < size ())
        erase (std::next (cbegin (), count), cend ());
    }

    template <std::size_t M, std::size_t J>
    void swap (partition_subrange<vector_partition<T, M, Container>, J>& other)
    {
      if (&other == this)
        return;

      container_type tmp (std::make_move_iterator (begin ()), std::make_move_iterator (end ()));
      assign (std::make_move_iterator (other.begin ()), std::make_move_iterator (other.end ()));
      other.assign (std::make_move_iterator (tmp.begin ()), std::make_move_iterator (tmp.end ()));
    }

    subrange_view<iter> view (void)
    {
      return { begin (), end () };
    }

    subrange_view<citer> view (void) const
    {
      return { begin (), end () };
    }

    iter advance_begin (diff_t change)
    {
      // first case: past end, second case: past begin
      if (((change > 0) && (static_cast<size_t> (change) > m_container.size () - m_offset)) ||
          ((change < 0) && (static_cast<size_t> (-change) > m_offset)))
        throw std::out_of_range ("requested change of subrange offset is out of range");

      m_offset += change;
      if (change > 0)
        next_subrange (*this).propagate_offset_right (m_offset);
      else
        prev_subrange (*this).propagate_offset_left (m_offset);
      return begin ();
    }

    template <std::size_t J = Index, typename std::enable_if<(J < N - 1)>::type * = nullptr>
    iter advance_end (diff_t change)
    {
      return next_subrange (*this).advance_begin (change);
    }

    template <std::size_t J = Index, typename std::enable_if<(J == N - 1)>::type * = nullptr>
    iter advance_end (diff_t change) = delete;

  protected:
    void partition_swap (partition_subrange& other)
      noexcept (next_type::template is_nothrow_swappable<size_t>::value &&
                noexcept (std::declval<next_type&> ().partition_swap (other)))
    {
      using std::swap;
      swap (m_offset, other.m_offset);
      next_type::partition_swap (other);
    }

    constexpr size_t get_offset (void) const noexcept { return m_offset; }

  private:
    void modify_offsets (diff_t change)
    {
      if (change == 0)
        return;

      next_subrange (*this).add_to_offset (change);
    }

    void add_to_offset (diff_t change)
    {
      next_subrange (*this).add_to_offset (change);
      m_offset += change;
    }

    void propagate_offset_left (size_t pos_off) noexcept
    {
      if (m_offset > pos_off)
      {
        m_offset = pos_off;
        prev_subrange (*this).propagate_offset_left (pos_off);
      }
    }

    void propagate_offset_right (size_t pos_off) noexcept
    {
      if (m_offset < pos_off)
      {
        m_offset = pos_off;
        next_subrange (*this).propagate_offset_right (pos_off);
      }
    }

    size_t m_offset = 0;
  };

  // end case holds the actual container
  template <typename T, std::size_t N, typename Container>
  class partition_subrange<vector_partition<T, N, Container>, N>
  {
    template <typename, std::size_t, typename>
    friend class partition_subrange;

  public:
    using partition_type = list_partition<T, N, Container>;
    using subrange_type  = partition_subrange<partition_type, N>;
    using prev_type      = partition_subrange<partition_type, N - 1>;
//  using next_type      = void;

    using container_type         = Container;
    using iterator               = typename Container::iterator;
    using const_iterator         = typename Container::const_iterator;
    using reverse_iterator       = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;
    using reference              = typename Container::reference;
    using const_reference        = typename Container::const_reference;
    using size_type              = typename Container::size_type;
    using difference_type        = typename Container::difference_type;
    using value_type             = typename Container::value_type;
    using allocator_type         = typename Container::allocator_type;

  private:
    using iter    = iterator;
    using citer   = const_iterator;
    using riter   = reverse_iterator;
    using criter  = const_reverse_iterator;
    using ref     = reference;
    using cref    = const_reference;
    using size_t  = size_type;
    using diff_t  = difference_type;
    using value_t = value_type;
    using alloc_t = allocator_type;

  public:
    partition_subrange            (void)                          = default;
    partition_subrange            (const partition_subrange&)     = default;
    partition_subrange            (partition_subrange&&) noexcept = default;
    partition_subrange& operator= (const partition_subrange&)     = default;
    partition_subrange& operator= (partition_subrange&&) noexcept = default;
    ~partition_subrange           (void)                          = default;

  protected:
    template <std::size_t M>
    GCH_CPP20_CONSTEXPR
    partition_subrange (size_t accum,
                        const partition_subrange<vector_partition<T, M, Container>, M>& other)
          GCH_CPP17_NOEXCEPT
      : m_container { }
    {
      m_container.reserve (accum + other.m_container.size ());
    }

    template <std::size_t M>
    GCH_CPP20_CONSTEXPR
    partition_subrange (size_t accum,
                        partition_subrange<vector_partition<T, M, Container>, M>&& other)
          GCH_CPP17_NOEXCEPT
      : m_container { }
    {
      m_container.reserve (accum + other.m_container.size ());
    }

    GCH_CPP20_CONSTEXPR explicit partition_subrange (const alloc_t& alloc) GCH_CPP17_NOEXCEPT
      : m_container (alloc)
    { }

  public:
    GCH_NODISCARD GCH_CPP20_CONSTEXPR iter  begin  (void)       noexcept { return m_container.end ();  }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer begin  (void) const noexcept { return m_container.cend (); }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR citer cbegin (void) const noexcept { return m_container.cend (); }

    iter   end     (void)       = delete;
    iter   end     (void) const = delete;
    citer  cend    (void)       = delete;

    GCH_NODISCARD GCH_CPP20_CONSTEXPR riter  rend   (void)       noexcept { return m_container.rbegin ();  }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter rend   (void) const noexcept { return m_container.crbegin (); }
    GCH_NODISCARD GCH_CPP20_CONSTEXPR criter crend  (void) const noexcept { return m_container.crbegin (); }

    iter   rbegin  (void)       = delete;
    iter   rbegin  (void) const = delete;
    citer  crbegin (void)       = delete;

    ref&   front   (void)       = delete;
    cref&  front   (void) const = delete;

    ref&   back    (void)       = delete;
    cref&  back    (void) const = delete;

    GCH_CPP20_CONSTEXPR alloc_t get_allocator (void) const noexcept
    {
      return m_container.get_allocator ();
    }

    GCH_NODISCARD GCH_CPP20_CONSTEXPR size_t max_size (void) const noexcept
    {
      return m_container.max_size ();
    }

    subrange_view<iter>  view (void)       = delete;
    subrange_view<citer> view (void) const = delete;

    iter advance_begin (diff_t) = delete;
    iter advance_end   (diff_t) = delete;

  protected:
    template <typename U>
    struct is_nothrow_swappable
    {
    private:
      static constexpr bool test (void)
      {
        using std::swap;
        return noexcept (swap (std::declval<U&> (), std::declval<U&> ()));
      }
    public:
      static constexpr bool value = test ();
    };

    void partition_swap (partition_subrange& other)
      noexcept (is_nothrow_swappable<container_type>::value)
    {
      using std::swap;
      swap (m_container, other.m_container);
    }

    constexpr size_t get_offset (void) const noexcept { return m_container.size (); }

  private:
    static void add_to_offset          (diff_t) noexcept { }
    static void propagate_offset_right (size_t) noexcept { }

  protected:
    container_type m_container;
  };

  template <typename T, std::size_t N, typename C, std::size_t I>
  void swap (partition_subrange<vector_partition<T, N, C>, I>& lhs,
             partition_subrange<vector_partition<T, N, C>, I>& rhs)
    noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  void swap (partition_subrange<vector_partition<T, N, C>, I>& lhs,
             partition_subrange<vector_partition<T, M, C>, J>& rhs)
    noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }

  template <typename T, std::size_t N, typename C, std::size_t I, typename U>
  typename partition_subrange<vector_partition<T, N, C>, I>::size_type
  erase (partition_subrange<vector_partition<T, N, C>, I>& c, const U& val)
  {
    auto it = std::remove (c.begin (), c.end (), val);
    auto r  = std::distance (it, c.end ());
    c.erase (it, c.end ());
    return r;
  }

  template <typename T, std::size_t N, typename C, std::size_t I, typename Pred>
  typename partition_subrange<vector_partition<T, N, C>, I>::size_type
  erase_if (partition_subrange<vector_partition<T, N, C>, I>& c, Pred pred)
  {
    auto it = std::remove_if (c.begin(), c.end(), pred);
    auto r  = std::distance (it, c.end());
    c.erase (it, c.end ());
    return r;
  }

  template <typename T, std::size_t N, typename Container>
  class vector_partition
    : public partition_traits<vector_partition<T, N, Container>>,
      protected partition_subrange<vector_partition<T, N, Container>, 0>
  {
  public:
    using traits = partition_traits<vector_partition>;

    using container_type = typename traits::container_type;

    using data_iter    = typename traits::data_iterator;
    using data_citer   = typename traits::data_const_iterator;
    using data_riter   = typename traits::data_reverse_iterator;
    using data_criter  = typename traits::data_const_reverse_iterator;
    using data_ref     = typename traits::data_reference;
    using data_cref    = typename traits::data_const_reference;
    using data_size_t  = typename traits::data_size_type;
    using data_diff_t  = typename traits::data_difference_type;
    using data_val_t   = typename traits::data_value_type;
    using data_alloc_t = typename traits::data_allocator_type;

    using first_type = partition_subrange<vector_partition, 0>;
    using last_type  = partition_subrange<vector_partition, N>;

    template <std::size_t Index>
    using subrange_type = partition_subrange<vector_partition, Index>;

  protected:
    using last_type::m_container;

  public:
    vector_partition            (void)                        = default;
    vector_partition            (const vector_partition&)     = default;
    vector_partition            (vector_partition&&) noexcept = default;
    vector_partition& operator= (const vector_partition&)     = default;
    vector_partition& operator= (vector_partition&&) noexcept = default;
    ~vector_partition           (void)                        = default;

    // concatenation constructor
    template <std::size_t M, typename ...Partitions>
    vector_partition (const vector_partition<T, M, Container>& p, Partitions&&... ps)
      : first_type (get_subrange<0> (p), std::forward<Partitions> (ps)...)
    {
      construct (p, std::forward<Partitions> (ps)...);
    }

    template <std::size_t M, typename ...Partitions>
    vector_partition (vector_partition<T, M, Container>&& p, Partitions&&... ps)
      : first_type (get_subrange<0> (p), std::forward<Partitions> (ps)...)
    {
      construct (std::move (p), std::forward<Partitions> (ps)...);
    }

  private:
    template <std::size_t M, typename ...Partitions>
    void construct (const vector_partition<T, M, Container>& p, Partitions&&... ps)
    {
      m_container.insert (m_container.cend (), p.data_begin (), p.data_end ());
      construct (std::forward<Partitions> (ps)...);
    }

    template <std::size_t M, typename ...Partitions>
    void construct (vector_partition<T, M, Container>&& p, Partitions&&... ps)
    {
      m_container.insert (m_container.cend (), std::make_move_iterator (p.data_begin ()),
                          std::make_move_iterator (p.data_end ()));
      construct (std::forward<Partitions> (ps)...);
    }

    static void construct (void) noexcept { }

  public:
    constexpr explicit vector_partition (const data_alloc_t& alloc)
      : first_type (alloc)
    { }

    template <std::size_t I, template <typename, std::size_t, typename> class PartitionT,
              typename U, std::size_t M, typename C>
    friend constexpr partition_element_t<I, PartitionT<U, M, C>>&
    get_subrange (PartitionT<U, M, C>& p) noexcept;

    template <std::size_t I, template <typename, std::size_t, typename> class PartitionT,
              typename U, std::size_t M, typename C>
    friend constexpr const partition_element_t<I, PartitionT<U, M, C>>&
    get_subrange (const PartitionT<U, M, C>& p) noexcept;

    template <std::size_t I, template <typename, std::size_t, typename> class PartitionT,
              typename U, std::size_t M, typename C>
    friend constexpr partition_element_t<I, PartitionT<U, M, C>>&&
    get_subrange (PartitionT<U, M, C>&& p) noexcept;

    template <std::size_t I, template <typename, std::size_t, typename> class PartitionT,
              typename U, std::size_t M, typename C>
    friend constexpr const partition_element_t<I, PartitionT<U, M, C>>&&
    get_subrange (const PartitionT<U, M, C>&& p) noexcept;

    template <typename Partition, std::size_t Index>
    friend constexpr partition_type_t<partition_subrange<Partition, Index>>&
    get_partition (partition_subrange<Partition, Index>& p) noexcept;

    template <typename Partition, std::size_t Index>
    friend constexpr const partition_type_t<partition_subrange<Partition, Index>>&
    get_partition (const partition_subrange<Partition, Index>& p) noexcept;

    template <typename Partition, std::size_t Index>
    friend constexpr partition_type_t<partition_subrange<Partition, Index>>&&
    get_partition (partition_subrange<Partition, Index>&& p) noexcept;

    template <typename Partition, std::size_t Index>
    friend constexpr const partition_type_t<partition_subrange<Partition, Index>>&&
    get_partition (const partition_subrange<Partition, Index>&& p) noexcept;

    GCH_CPP14_CONSTEXPR subrange_type<0> front (void) noexcept
    {
      return get_subrange<0> (*this);
    }

    constexpr subrange_type<0> front (void) const noexcept
    {
      return get_subrange<0> (*this);
    }

    GCH_CPP14_CONSTEXPR subrange_type<N - 1> back (void) noexcept
    {
      return get_subrange<N - 1> (*this);
    }

    constexpr subrange_type<N - 1> back (void) const noexcept
    {
      return get_subrange<N - 1> (*this);
    }

    static constexpr std::size_t size (void) noexcept { return N; }
    GCH_NODISCARD static constexpr bool empty (void) noexcept { return N == 0; }

    data_iter   data_begin   (void)       noexcept { return m_container.begin ();   }
    data_citer  data_begin   (void) const noexcept { return m_container.begin ();   }
    data_citer  data_cbegin  (void) const noexcept { return m_container.cbegin ();  }

    data_iter   data_end     (void)       noexcept { return m_container.end ();     }
    data_citer  data_end     (void) const noexcept { return m_container.end ();     }
    data_citer  data_cend    (void) const noexcept { return m_container.cend ();    }

    data_riter  data_rbegin  (void)       noexcept { return m_container.rbegin ();  }
    data_criter data_rbegin  (void) const noexcept { return m_container.rbegin ();  }
    data_criter data_crbegin (void) const noexcept { return m_container.crbegin (); }

    data_riter  data_rend    (void)       noexcept { return m_container.rend ();    }
    data_criter data_rend    (void) const noexcept { return m_container.rend ();    }
    data_criter data_crend   (void) const noexcept { return m_container.crend ();   }

    data_ref&   data_front   (void)       noexcept { return m_container.front ();   }
    data_cref&  data_front   (void) const noexcept { return m_container.front ();   }

    data_ref&   data_back    (void)       noexcept { return m_container.back ();    }
    data_cref&  data_back    (void) const noexcept { return m_container.back ();    }

    data_size_t data_size (void) const noexcept { return m_container.size (); }
    GCH_NODISCARD bool data_empty (void) const noexcept { return m_container.empty (); }

    gch::subrange_view<data_iter> data_view (void)
    {
      return { m_container.begin (), m_container.end () };
    }

    gch::subrange_view<data_citer> data_view (void) const
    {
      return { m_container.begin (), m_container.end () };
    }

    gch::partition_view<vector_partition, N> partition_view (void)
    {
      return gch::partition_view<vector_partition, N> (*this);
    }

    gch::partition_view<const vector_partition, N> partition_view (void) const
    {
      return gch::partition_view<const vector_partition, N> (*this);
    }

    template <std::size_t Idx>
    gch::subrange_view<data_iter> subrange_view (void)
    {
      return get_subrange<Idx> (*this).view ();
    }

    template <std::size_t Idx>
    gch::subrange_view<data_citer> subrange_view (void) const
    {
      return get_subrange<Idx> (*this).view ();
    }

    template <std::size_t Index,
              typename = typename std::enable_if<(0 < Index) && (Index < N)>::type>
    data_iter advance_begin (data_diff_t change)
    {
      return get_subrange<Index> (*this).advance_begin (change);
    }

    template <std::size_t Index,
              typename = typename std::enable_if<(Index < N)>::type>
    data_iter advance_end (data_diff_t change)
    {
      return get_subrange<Index> (*this).advance_end (change);
    }

    void swap (vector_partition& other)
      noexcept (noexcept (std::declval<vector_partition&> ().partition_swap (other)))
    {
      first_type::partition_swap (other);
    }

#ifdef GCH_PARTITION_ITERATOR

    using iter   = partition_iterator<vector_partition>;
    using citer  = partition_iterator<const vector_partition>;
    using riter  = std::reverse_iterator<iter>;
    using criter = std::reverse_iterator<citer>;

    [[nodiscard]] constexpr iter  begin   (void)       noexcept { return { *this, 0 };       }
    [[nodiscard]] constexpr citer begin   (void) const noexcept { return { *this, 0 };       }
    [[nodiscard]] constexpr citer cbegin  (void) const noexcept { return { *this, 0 };       }

    [[nodiscard]] constexpr iter  end     (void)       noexcept { return { *this, N };       }
    [[nodiscard]] constexpr citer end     (void) const noexcept { return { *this, N };       }
    [[nodiscard]] constexpr citer cend    (void) const noexcept { return { *this, N };       }

    [[nodiscard]] constexpr auto  rbegin  (void)       noexcept { return riter (end ());     }
    [[nodiscard]] constexpr auto  rbegin  (void) const noexcept { return criter (cend ());   }
    [[nodiscard]] constexpr auto  crbegin (void) const noexcept { return criter (cend ());   }

    [[nodiscard]] constexpr auto  rend    (void)       noexcept { return riter (begin ());   }
    [[nodiscard]] constexpr auto  rend    (void) const noexcept { return criter (cbegin ()); }
    [[nodiscard]] constexpr auto  crend   (void) const noexcept { return criter (cbegin ()); }

    template <typename ...Fs>
    static constexpr auto overload (Fs&&... fs) noexcept
    {
      return partition_overloader<vector_partition, Fs...> (std::forward<Fs> (fs)...);
    }

#endif

  private:
  };

  template <typename T, std::size_t N, typename Container>
  void swap (vector_partition<T, N, Container>& lhs, vector_partition<T, N, Container>& rhs)
  {
    lhs.swap (rhs);
  }

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  constexpr bool operator== (const partition_subrange<vector_partition<T, N, C>, I>& lhs,
                             const partition_subrange<vector_partition<T, M, C>, J>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  bool operator<= (const partition_subrange<vector_partition<T, N, C>, I>& lhs,
                   const partition_subrange<vector_partition<T, M, C>, J>& rhs)
  {
    return std::lexicographical_compare_three_way (lhs.begin (), lhs.end (),
                                                   rhs.begin (), rhs.end (),
                                                   detail::compare::synth_three_way);
  }

#else

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  bool operator== (const partition_subrange<vector_partition<T, N, C>, I>& lhs,
                   const partition_subrange<vector_partition<T, M, C>, J>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  bool operator!= (const partition_subrange<vector_partition<T, N, C>, I>& lhs,
                   const partition_subrange<vector_partition<T, M, C>, J>& rhs)
  {
    return ! (lhs == rhs);
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  bool operator< (const partition_subrange<vector_partition<T, N, C>, I>& lhs,
                  const partition_subrange<vector_partition<T, M, C>, J>& rhs)
  {
    return std::lexicographical_compare (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  bool operator<= (const partition_subrange<vector_partition<T, N, C>, I>& lhs,
                   const partition_subrange<vector_partition<T, M, C>, J>& rhs)
  {
    return ! (lhs > rhs);
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  bool operator> (const partition_subrange<vector_partition<T, N, C>, I>& lhs,
                  const partition_subrange<vector_partition<T, M, C>, J>& rhs)
  {
    return std::lexicographical_compare (rhs.begin (), rhs.end (), lhs.begin (), lhs.end ());
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  bool operator>= (const partition_subrange<vector_partition<T, N, C>, I>& lhs,
                   const partition_subrange<vector_partition<T, M, C>, J>& rhs)
  {
    return ! (lhs < rhs);
  }

#endif
}

#endif // PARTITION_VECTOR_PARTITION_HPP
