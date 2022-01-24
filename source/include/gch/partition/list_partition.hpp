/** list_partition.hpp
 * Short description here.
 *
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GCH_PARTITION_LIST_PARTITION_HPP
#define GCH_PARTITION_LIST_PARTITION_HPP

#include "partition.hpp"

#include <stdexcept>
#include <list>

namespace gch
{

  template <typename T, std::size_t N, typename Container = std::list<T>>
  class list_partition;

  template <typename T, std::size_t N, typename Container, std::size_t Index>
  class partition_subrange<list_partition<T, N, Container>, Index,
    typename std::enable_if<! (0 <= Index || Index <= N || Index == partition_base_index)>::type>
  {
    static_assert (0 <= Index || Index <= N || Index == partition_base_index,
                   "Invalid partition subrange index.");
  };

  template <typename T, std::size_t N, typename Container>
  class partition_subrange<list_partition<T, N, Container>, 0>
    : public partition_subrange<list_partition<T, N, Container>, 1>
  {
    template <typename, std::size_t, typename>
    friend class partition_subrange;

  public:
    using partition_type = list_partition<T, N, Container>;
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

    partition_subrange            (void)                          = default;
    partition_subrange            (const partition_subrange&)     = default;
    partition_subrange            (partition_subrange&&) noexcept = default;
//  partition_subrange& operator= (const partition_subrange&)     = impl;
//  partition_subrange& operator= (partition_subrange&&) noexcept = impl;
    ~partition_subrange           (void)                          = default;

    partition_subrange&
    operator= (const partition_subrange& other) noexcept
    {
      if (&other != this)
        assign (other.begin (), other.end ());
      return *this;
    }

    partition_subrange&
    operator= (partition_subrange&& other) noexcept
    {
      if (! other.empty ())
      {
        iter last = other.begin ();
        splice (cend (), std::move (other));
        erase (begin (), last);
      }
      return *this;
    }

    template <std::size_t M, typename ...Subranges>
    constexpr
    partition_subrange (const partition_subrange<list_partition<T, M, Container>, 0>& other,
                        Subranges&&... subranges)
      : next_type (next_subrange (other), std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, typename ...Subranges>
    constexpr
    partition_subrange (partition_subrange<list_partition<T, M, Container>, 0>&& other,
                        Subranges&&... subranges)
      : next_type (next_subrange (other), std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, typename ...Subranges>
    constexpr
    partition_subrange (list_partition<T, M, Container>&& partition, Subranges&&... subranges)
      : partition_subrange (get_subrange<0> (partition), std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, typename ...Subranges>
    constexpr
    partition_subrange (const list_partition<T, M, Container>& partition, Subranges&&... subranges)
      : partition_subrange (get_subrange<0> (partition), std::forward<Subranges> (subranges)...)
    { }

    constexpr explicit
    partition_subrange (const alloc_t& alloc)
      : next_type (alloc)
    { }

    explicit
    partition_subrange (iter) { }

  public:
    void
    assign (size_t count, const value_t& val)
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
    void
    assign (It first, It last)
    {
      iter curr = begin ();
      for (; curr != end () && first != last; ++curr, (void)++first)
        *curr = *first;

      if (first == last)
        erase(curr, end ());
      else
        insert(end (), first, last);
    }

    void
    assign (std::initializer_list<value_t> ilist)
    {
      assign (ilist.begin (), ilist.end ());
    }

    GCH_NODISCARD iter   begin  (void)       noexcept { return m_container.begin ();  }
    GCH_NODISCARD citer  begin  (void) const noexcept { return m_container.cbegin (); }
    GCH_NODISCARD citer  cbegin (void) const noexcept { return m_container.cbegin (); }

    GCH_NODISCARD iter   end     (void)       noexcept { return next_type::begin ();  }
    GCH_NODISCARD citer  end     (void) const noexcept { return next_type::cbegin (); }
    GCH_NODISCARD citer  cend    (void) const noexcept { return next_type::cbegin (); }

    GCH_NODISCARD riter  rbegin  (void)       noexcept { return next_type::rend ();   }
    GCH_NODISCARD criter rbegin  (void) const noexcept { return next_type::crend ();  }
    GCH_NODISCARD criter crbegin (void) const noexcept { return next_type::crend ();  }

    GCH_NODISCARD riter  rend    (void)       noexcept { return m_container.rend ();  }
    GCH_NODISCARD criter rend    (void) const noexcept { return m_container.crend (); }
    GCH_NODISCARD criter crend   (void) const noexcept { return m_container.crend (); }

    GCH_NODISCARD ref&   front   (void)       noexcept { return *begin ();            }
    GCH_NODISCARD cref&  front   (void) const noexcept { return *begin ();            }

    GCH_NODISCARD ref&   back    (void)       noexcept { return *(--end ());          }
    GCH_NODISCARD cref&  back    (void) const noexcept { return *(--cend ());         }

    GCH_NODISCARD GCH_CPP17_CONSTEXPR
    std::size_t
    size (void) const noexcept
    {
      return std::distance (cbegin (), cend ());
    }

    GCH_NODISCARD
    bool
    empty (void) const noexcept
    {
      return cbegin () == cend ();
    }

    void
    clear (void) noexcept
    {
      erase (cbegin (), cend ());
    }

    iter
    insert (const citer pos, const value_t& lv)
    {
      return m_container.insert (pos, lv);
    }

    iter
    insert (const citer pos, value_t&& rv)
    {
      return m_container.insert (pos, std::move (rv));
    }

    iter
    insert (const citer pos, size_t count, const value_t& val)
    {
      return m_container.insert (pos, count, val);
    }

    template <typename Iterator>
    iter
    insert (const citer pos, Iterator first, Iterator last)
    {
      return m_container.insert (pos, first, last);
    }

    iter
    insert (const citer pos, std::initializer_list<value_t> ilist)
    {
      return m_container.insert (pos, ilist);
    }

    template <typename ...Args>
    iter
    emplace (const citer pos, Args&&... args)
    {
      return m_container.emplace (pos, std::forward<Args> (args)...);
    }

    iter
    erase (const citer pos)
    {
      return m_container.erase (pos);
    }

    iter
    erase (const citer first, const citer last)
    {
      return m_container.erase (first, last);
    }

    void
    push_back (const value_t& val)
    {
      insert (cend (), val);
    }

    void
    push_back (value_t&& val)
    {
      insert (cend (), std::move (val));
    }

    template <typename ...Args>
    ref
    emplace_back (Args&&... args)
    {
      return *emplace (cend (), std::forward<Args> (args)...);
    }

    void
    pop_back (void)
    {
      erase (--cend ());
    }

    void
    push_front (const value_t& val)
    {
      m_container.push_front (val);
    }

    void
    push_front (value_t&& val)
    {
      m_container.push_front (std::move (val));
    }

    template <typename ...Args>
    ref
    emplace_front (Args&&... args)
    {
      return *emplace (cbegin (), std::forward<Args> (args)...);
    }

    void
    pop_front (void)
    {
      m_container.pop_front ();
    }

    template <std::size_t M, std::size_t J>
    void
    swap (partition_subrange<list_partition<T, M, Container>, J>& other)
    {
      if (&other == this)
        return;

      iter tmp_first = begin ();

      other.m_container.splice (other.cend (), std::move (m_container), cbegin (), cend ());
      m_container.splice (cend (), std::move (other.m_container), other.cbegin (), tmp_first);

      other.set_first (tmp_first);
    }

    template <std::size_t M>
    void
    swap (partition_subrange<list_partition<T, M, Container>, 0>& other)
    {
      if (&other == this)
        return;

      iter tmp_first = begin ();

      other.m_container.splice (other.cend (), std::move (m_container), cbegin (), cend ());
      m_container.splice (cend (), std::move (other.m_container), other.cbegin (), tmp_first);
    }

    template <std::size_t M, std::size_t J, typename ...Args>
    void
    merge (partition_subrange<list_partition<T, M, Container>, J>& other, Args&&... args)
    {
      merge (std::move (other), std::forward<Args> (args)...);
    }

    template <std::size_t M, std::size_t J, typename ...Args>
    void
    merge (partition_subrange<list_partition<T, M, Container>, J>&& other, Args&&... args)
    {
      if (&other == this)
        return;

      container_type tmp1;
      container_type tmp2;
      tmp1.splice (tmp1.cend (), std::move (m_container), cbegin (), cend ());
      tmp2.splice (tmp2.cend (), std::move (other.m_container), other.cbegin (), other.cend ());
      tmp1.merge (std::move (tmp2), std::forward<Args> (args)...);
      m_container.splice (cend (), tmp1);
      other.set_first (other.end ());
    }

    template <std::size_t M, std::size_t J, typename ...Args>
    void
    splice (citer pos, partition_subrange<list_partition<T, M, Container>, J>& other,
                 Args&&... args)
    {
      splice (pos, std::move (other), std::forward<Args> (args)...);
    }

    template <std::size_t M, std::size_t J>
    void
    splice (citer pos, partition_subrange<list_partition<T, M, Container>, J>&& other)
    {
      m_container.splice (pos, std::move (other.m_container), other.cbegin (), other.cend ());
      other.set_first (other.end ());
    }

    template <std::size_t M, std::size_t J>
    void
    splice (citer pos, partition_subrange<list_partition<T, M, Container>, J>&& other,
            citer cit)
    {
      citer new_other_front = std::next (cit);
      m_container.splice (pos, std::move (other.m_container), cit);
      other.propagate_first_left (cit, get_iter (new_other_front));
    }

    template <std::size_t M, std::size_t J>
    void
    splice (citer pos, partition_subrange<list_partition<T, M, Container>, J>&& other,
            citer first, citer last)
    {
      m_container.splice (pos, std::move (other.m_container), first, last);
      other.propagate_first_left (first, get_iter (last));
    }

    size_t
    remove (const T& val)
    {
      return remove_if ([&val] (cref elem) { return elem == val; });
    }

    template <typename UnaryPredicate>
    size_t
    remove_if (UnaryPredicate p)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());

      size_t original_size = tmp.size ();
      tmp.remove_if (p);
      size_t num_removed = original_size - tmp.size ();

      m_container.splice (cend (), tmp);

      return num_removed;
    }

    template <typename ...Args>
    void
    unique (Args&&... args)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      tmp.unique (std::forward<Args> (args)...);
      m_container.splice (cend (), tmp);
    }

    template <typename ...Args>
    void
    sort (Args&&... args)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      tmp.sort (std::forward<Args> (args)...);
      m_container.splice (cend (), tmp);
    }

    void
    resize (size_t count)
    {
      std::pair<citer, size_t> pos = resize_pos (count);
      if (pos.second != 0)
      {
        size_t idx = 0;
        try
        {
          for (; idx < pos.second; ++idx)
            emplace_back ();
        }
        catch (...)
        {
          for (; idx != 0; --idx)
            pop_back ();
          throw;
        }
      }
      else
        erase (pos.first, cend ());
    }

    void
    resize (size_t count, const value_t& val)
    {
      std::pair<citer, size_t> pos = resize_pos (count);
      if (pos.second != 0)
        insert (cend (), pos.second, val);
      else
        erase (pos.first, cend ());
    }

    subrange_view<iter>
    view (void)
    {
      return { begin (), end () };
    }

    subrange_view<citer>
    view (void) const
    {
      return { begin (), end () };
    }

    iter
    advance_begin (diff_t) = delete;

    iter
    advance_end (diff_t change)
    {
      return next_subrange (*this).advance_begin (change);
    }

  protected:
    void
    partition_swap (partition_subrange& other)
      noexcept (noexcept (std::declval<next_type&> ().partition_swap (other)))
    {
      next_type::partition_swap (other);
    }

  private:
    static void set_first            (iter)        noexcept { }
    static void propagate_first_left (citer, iter) noexcept { }

    std::pair<citer, size_t>
    resize_pos (const size_t count) const
    {
      const size_t len = size ();
      if (count < len)
      {
        if (count <= len / 2)
          return { std::next (cbegin (), count), 0 };
        else
          return { std::next (cend (), static_cast<diff_t> (count - len)), 0 };
      }
      return { cend (), count - len };
    }

  };

  template <typename T, std::size_t N, typename Container, std::size_t Index>
  class partition_subrange<list_partition<T, N, Container>, Index,
                           typename std::enable_if<(0 < Index) && (Index < N)>::type>
    : public partition_subrange<list_partition<T, N, Container>, Index + 1>
  {
    template <typename, std::size_t, typename>
    friend class partition_subrange;

  public:
    using partition_type = list_partition<T, N, Container>;
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

//  partition_subrange            (void)                          = impl;
//  partition_subrange            (const partition_subrange&)     = impl;
//  partition_subrange            (partition_subrange&&) noexcept = impl;
//  partition_subrange& operator= (const partition_subrange&)     = impl;
//  partition_subrange& operator= (partition_subrange&&) noexcept = impl;
    ~partition_subrange           (void)                          = default;

    partition_subrange (void)
      : next_type (),
        m_first (m_container.end ())
    { }

    partition_subrange (const partition_subrange& other)
      : next_type (other),
        m_first (std::next (m_container.begin (),
                            std::distance (other.m_container.begin (), other.begin ())))
    { }

    partition_subrange (partition_subrange&& other)
      : next_type (std::move (other)),
        m_first (other.m_first)
    { }

    partition_subrange&
    operator= (const partition_subrange& other) noexcept
    {
      if (&other != this)
        assign (other.begin (), other.end ());
      return *this;
    }

    partition_subrange&
    operator= (partition_subrange&& other) noexcept
    {
      if (! other.empty ())
      {
        iter last = other.begin ();
        splice (cend (), std::move (other));
        erase (begin (), last);
      }
      return *this;
    }

    template <std::size_t M, std::size_t J, typename ...Subranges,
      typename std::enable_if<(J < M)>::type * = nullptr>
    constexpr
    partition_subrange (const partition_subrange<list_partition<T, M, Container>, J>& other,
                        Subranges&&... subranges)
      : next_type (next_subrange (other), std::forward<Subranges> (subranges)...),
        m_first (std::next (m_container.begin (),
                            std::distance (other.m_container.begin (), other.begin ())))
    { }

    template <std::size_t M, typename ...Subranges>
    partition_subrange (const partition_subrange<list_partition<T, M, Container>, M>& other,
                             Subranges&&... subranges)
      : partition_subrange (std::forward<Subranges> (subranges)...)
    {
      m_container.insert (m_container.cbegin (), other.m_container.cbegin (),
                          other.m_container.cend ());
    }

    template <std::size_t M, std::size_t J, typename ...Subranges,
              typename std::enable_if<(J < M)>::type * = nullptr>
    constexpr
    partition_subrange (partition_subrange<list_partition<T, M, Container>, J>&& other,
                             Subranges&&... subranges)
      : next_type (next_subrange (other), std::forward<Subranges> (subranges)...),
        m_first (other.begin ())
    { }

    template <std::size_t M, typename ...Subranges>
    partition_subrange (partition_subrange<list_partition<T, M, Container>, M>&& other,
                             Subranges&&... subranges)
      : partition_subrange (std::forward<Subranges> (subranges)...)
    {
      m_container.splice (m_container.cbegin (), other.m_container);
    }

    template <std::size_t M, typename ...Subranges>
    constexpr
    partition_subrange (list_partition<T, M, Container>&& partition,
                                       Subranges&&... subranges)
      : partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }

    template <std::size_t M, typename ...Subranges>
    constexpr
    partition_subrange (const list_partition<T, M, Container>& partition,
                                       Subranges&&... subranges)
      : partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }

    constexpr explicit
    partition_subrange (const alloc_t& alloc)
      : next_type (alloc)
    { }

  public:
    void
    assign (size_t count, const value_t& val)
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
    void
    assign (It first, It last)
    {
      iter curr = begin ();
      for (; curr != end () && first != last; ++curr, (void)++first)
        *curr = *first;

      if (first == last)
        erase(curr, end ());
      else
        insert(end (), first, last);
    }

    void
    assign (std::initializer_list<value_t> ilist)
    {
      assign (ilist.begin (), ilist.end ());
    }

    GCH_NODISCARD iter  begin  (void)       noexcept { return m_first;                }
    GCH_NODISCARD citer begin  (void) const noexcept { return citer (m_first);        }
    GCH_NODISCARD citer cbegin (void) const noexcept { return citer (m_first);        }

    GCH_NODISCARD iter   end     (void)       noexcept { return next_type::begin ();  }
    GCH_NODISCARD citer  end     (void) const noexcept { return next_type::cbegin (); }
    GCH_NODISCARD citer  cend    (void) const noexcept { return next_type::cbegin (); }

    GCH_NODISCARD riter  rbegin  (void)       noexcept { return next_type::rend ();   }
    GCH_NODISCARD criter rbegin  (void) const noexcept { return next_type::crend ();  }
    GCH_NODISCARD criter crbegin (void) const noexcept { return next_type::crend ();  }

    GCH_NODISCARD riter  rend    (void)       noexcept { return riter (begin ());     }
    GCH_NODISCARD criter rend    (void) const noexcept { return criter (cbegin ());   }
    GCH_NODISCARD criter crend   (void) const noexcept { return criter (cbegin ());   }

    GCH_NODISCARD ref&   front   (void)       noexcept { return *begin ();            }
    GCH_NODISCARD cref&  front   (void) const noexcept { return *begin ();            }

    GCH_NODISCARD ref&   back    (void)       noexcept { return *(--end ());          }
    GCH_NODISCARD cref&  back    (void) const noexcept { return *(--cend ());         }

    GCH_NODISCARD
    std::size_t
    size (void) const noexcept
    {
      return static_cast<std::size_t> (std::distance (cbegin (), cend ()));
    }

    GCH_NODISCARD
    bool
    empty (void) const noexcept
    {
      return cbegin () == cend ();
    }

    void
    clear (void) noexcept
    {
      set_first (m_container.erase (cbegin (), cend ()));
    }

    iter
    insert (const citer pos, const value_t& lv)
    {
      iter ret = m_container.insert (pos, lv);
      propagate_first_left (pos, ret);
      return ret;
    }

    iter
    insert (const citer pos, value_t&& rv)
    {
      iter ret = m_container.insert (pos, std::move (rv));
      propagate_first_left (pos, ret);
      return ret;
    }

    iter
    insert (const citer pos, size_t count, const value_t& val)
    {
      iter ret = m_container.insert (pos, count, val);
      propagate_first_left (pos, ret);
      return ret;
    }

    template <typename Iterator>
    iter
    insert (const citer pos, Iterator first, Iterator last)
    {
      iter ret = m_container.insert (pos, first, last);
      propagate_first_left (pos, ret);
      return ret;
    }

    iter
    insert (const citer pos, std::initializer_list<value_t> ilist)
    {
      iter ret = m_container.insert (pos, ilist);
      propagate_first_left (pos, ret);
      return ret;
    }

    template <typename ...Args>
    iter
    emplace (const citer pos, Args&&... args)
    {
      iter ret = m_container.emplace (pos, std::forward<Args> (args)...);
      propagate_first_left (pos, ret);
      return ret;
    }

    iter
    erase (const citer pos)
    {
      iter ret = m_container.erase (pos);
      propagate_first_left (pos, ret);
      return ret;
    }

    iter
    erase (const citer first, const citer last)
    {
      iter ret = m_container.erase (first, last);
      propagate_first_left (first, ret);
      return ret;
    }

    void
    push_back (const value_t& val)
    {
      insert (cend (), val);
    }

    void
    push_back (value_t&& val)
    {
      insert (cend (), std::move (val));
    }

    template <typename ...Args>
    ref
    emplace_back (Args&&... args)
    {
      return *emplace (cend (), std::forward<Args> (args)...);
    }

    void
    pop_back (void)
    {
      erase (--cend ());
    }

    void
    push_front (const value_t& val)
    {
      set_first (m_container.insert (cbegin (), val));
    }

    void
    push_front (value_t&& val)
    {
      set_first (m_container.insert (cbegin (), std::move (val)));
    }

    template <typename ...Args>
    ref
    emplace_front (Args&&... args)
    {
      set_first (m_container.emplace (cbegin (), std::forward<Args> (args)...));
      return *begin ();
    }

    void
    pop_front (void)
    {
      set_first (m_container.erase (cbegin ()));
    }

    void
    resize (const size_t count)
    {
      std::pair<citer, size_t> pos = resize_pos (count);
      if (pos.second != 0)
      {
        size_t idx = 0;
        try
        {
          for (; idx < pos.second; ++idx)
            emplace_back ();
        }
        catch (...)
        {
          for (; idx != 0; --idx)
            pop_back ();
          throw;
        }
      }
      else
        erase (pos.first, cend ());
    }

    void
    resize (const size_t count, const value_t& val)
    {
      std::pair<citer, size_t> pos = resize_pos (count);
      if (pos.second != 0)
        insert (cend (), pos.second, val);
      else
        erase (pos.first, cend ());
    }

    template <std::size_t M, std::size_t J>
    void
    swap (partition_subrange<list_partition<T, M, Container>, J>& other)
    {
      if (&other == this)
        return;

      iter tmp_first = begin ();

      other.m_container.splice (other.cend (), std::move (m_container), cbegin (), cend ());
      m_container.splice (cend (), std::move (other.m_container), other.cbegin (), tmp_first);

      set_first (other.m_first);
      other.set_first (tmp_first);
    }

    template <std::size_t M>
    void
    swap (partition_subrange<list_partition<T, M, Container>, 0>& other)
    {
      if (&other == this)
        return;

      iter other_first = other.begin ();

      other.m_container.splice (other.cend (), std::move (m_container), cbegin (), cend ());
      m_container.splice (cend (), std::move (other.m_container), other.cbegin (), m_first);

      set_first (other_first);
    }

    template <std::size_t M, std::size_t J, typename ...Args>
    void
    merge (partition_subrange<list_partition<T, M, Container>, J>& other, Args&&... args)
    {
      merge (std::move (other), std::forward<Args> (args)...);
    }

    template <std::size_t M, std::size_t J, typename ...Args>
    void
    merge (partition_subrange<list_partition<T, M, Container>, J>&& other, Args&&... args)
    {
      if (&other == this)
        return;

      container_type tmp1;
      container_type tmp2;
      tmp1.splice (tmp1.cend (), std::move (m_container), cbegin (), cend ());
      tmp2.splice (tmp2.cend (), std::move (other.m_container), other.cbegin (), other.cend ());
      tmp1.merge (std::move (tmp2), std::forward<Args> (args)...);

      iter new_first = tmp1.begin ();
      m_container.splice (cend (), tmp1);
      set_first (new_first);
      other.set_first (other.end ());
    }

    template <std::size_t M, std::size_t J, typename ...Args>
    void
    splice (const citer pos, partition_subrange<list_partition<T, M, Container>, J>& other,
                 Args&&... args)
    {
      splice (pos, std::move (other), std::forward<Args> (args)...);
    }

    template <std::size_t M, std::size_t J>
    void
    splice (const citer pos, partition_subrange<list_partition<T, M, Container>, J>&& other)
    {
      iter first = other.begin ();
      m_container.splice (pos, std::move (other.m_container), first, other.cend ());
      other.set_first (other.end ());
      propagate_first_left (pos, first);
    }

    template <std::size_t M, std::size_t J>
    void
    splice (const citer pos, partition_subrange<list_partition<T, M, Container>, J>&& other,
            const citer cit)
    {
      citer new_other_front = std::next (cit);
      m_container.splice (pos, std::move (other.m_container), cit);
      other.propagate_first_left (cit, get_iter (new_other_front));
      propagate_first_left (pos, get_iter (cit));
    }

    template <std::size_t M, std::size_t J>
    void
    splice (const citer pos, partition_subrange<list_partition<T, M, Container>, J>&& other,
            const citer first, const citer last)
    {
      m_container.splice (pos, std::move (other.m_container), first, last);
      other.propagate_first_left (first, get_iter (last));
      propagate_first_left (pos, get_iter (first));
    }

    size_t
    remove (const T& val)
    {
      return remove_if ([&val] (cref elem) { return elem == val; });
    }

    template <typename UnaryPredicate>
    size_t
    remove_if (UnaryPredicate p)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());

      size_t original_size = tmp.size ();
      tmp.remove_if (p);
      size_t num_removed = original_size - tmp.size ();

      iter new_first = tmp.begin ();
      m_container.splice (cend (), tmp);
      set_first (new_first);

      return num_removed;
    }

    template <typename ...Args>
    void
    unique (Args&&... args)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      tmp.unique (std::forward<Args> (args)...);
      iter new_first = tmp.begin ();
      m_container.splice (cend (), tmp);
      set_first (new_first);
    }

    template <typename ...Args>
    void
    sort (Args&&... args)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      tmp.sort (std::forward<Args> (args)...);
      iter new_first = tmp.begin ();
      m_container.splice (cend (), tmp);
      set_first (new_first);
    }

    subrange_view<iter>
    view (void)
    {
      return { begin (), end () };
    }

    subrange_view<citer>
    view (void) const
    {
      return { begin (), end () };
    }

    iter
    advance_begin (diff_t change)
    {
      if (change > 0)
      {
        iter res = m_first;
        for (diff_t i = 0; i < change; ++i, static_cast<void> (++res))
        {
          if (res == m_container.end ())
            throw std::out_of_range ("The requested change of subrange offset is out of range.");
        }

        // rest should be noexcept
        while (m_first != res)
        {
          next_subrange (*this).propagate_first_right (m_first, std::next (m_first));
          std::advance (m_first, 1);
        }
      }
      else
      {
        iter res = m_first;
        for (diff_t i = 0; i > change; --i, static_cast<void> (--res))
        {
          if (res == m_container.begin ())
            throw std::out_of_range ("The requested change of subrange offset is out of range.");
        }

        // rest should be noexcept
        while (m_first != res)
        {
          prev_subrange (*this).propagate_first_left (m_first, std::prev (m_first));
          std::advance (m_first, -1);
        }
      }
      return m_first;
    }

    template <std::size_t J = Index, typename std::enable_if<(J < N - 1)>::type * = nullptr>
    iter
    advance_end (diff_t change)
    {
      return next_subrange (*this).advance_begin (change);
    }

    template <std::size_t J = Index, typename std::enable_if<(J == N - 1)>::type * = nullptr>
    iter
    advance_end (diff_t change) = delete;

  protected:
    void
    partition_swap (partition_subrange& other)
      noexcept (next_type::template is_nothrow_swappable<size_t>::value &&
                noexcept (std::declval<next_type&> ().partition_swap (other)))
    {
      using std::swap;
      swap (m_first, other.m_first);
      next_type::partition_swap (other);
    }

  private:
    iter
    get_iter (citer cit)
    {
      return m_container.erase (cit, cit);
    }

    void
    set_first (iter replace)
    {
      prev_subrange (*this).propagate_first_left (m_first, replace);
      m_first = replace;
    }

    void
    propagate_first_left (citer cmp, iter replace) noexcept
    {
      if (m_first == cmp)
      {
        prev_subrange (*this).propagate_first_left (cmp, replace);
        m_first = replace;
      }
    }

    void
    propagate_first_right (citer cmp, iter replace) noexcept
    {
      if (m_first == cmp)
      {
        next_subrange (*this).propagate_first_right (cmp, replace);
        m_first = replace;
      }
    }

    std::pair<citer, size_t>
    resize_pos (const size_t count) const
    {
      const size_t len = size ();
      if (count < len)
      {
        if (count <= len / 2)
          return { std::next (cbegin (), count), 0 };
        else
          return { std::next (cend (), static_cast<diff_t> (count - len)), 0 };
      }
      return { cend (), count - len };
    }

    iter m_first;
  };

  template <typename T, std::size_t N, typename Container>
  class partition_subrange<list_partition<T, N, Container>, N>
    : public partition_subrange<list_partition<T, N, Container>, partition_base_index>
  {
    using base = partition_subrange<list_partition<T, N, Container>, partition_base_index>;

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

  protected:
    using base::m_container;

    partition_subrange            (void)                          = default;
    partition_subrange            (const partition_subrange&)     = default;
    partition_subrange            (partition_subrange&&) noexcept = default;
    partition_subrange& operator= (const partition_subrange&)     = default;
    partition_subrange& operator= (partition_subrange&&) noexcept = default;
    ~partition_subrange           (void)                          = default;

    template <std::size_t M>
    constexpr
    partition_subrange (const partition_subrange<list_partition<T, M, Container>, M>& other)
      : base (get_base (other))
    { }

    template <std::size_t M>
    constexpr
    partition_subrange (partition_subrange<list_partition<T, M, Container>, M>&& other)
      : base (std::move (get_base (other)))
    { }

    constexpr explicit
    partition_subrange (const alloc_t& alloc)
      : base (alloc)
    { }

  public:
    GCH_NODISCARD iter   begin  (void)       noexcept { return m_container.end ();     }
    GCH_NODISCARD citer  begin  (void) const noexcept { return m_container.cend ();    }
    GCH_NODISCARD citer  cbegin (void) const noexcept { return m_container.cend ();    }

    GCH_NODISCARD riter  rend   (void)       noexcept { return m_container.rbegin ();  }
    GCH_NODISCARD criter rend   (void) const noexcept { return m_container.crbegin (); }
    GCH_NODISCARD criter crend  (void) const noexcept { return m_container.crbegin (); }

    alloc_t
    get_allocator (void) const noexcept
    {
      return m_container.get_allocator ();
    }

    GCH_NODISCARD
    size_t
    max_size (void) const noexcept
    {
      return m_container.max_size ();
    }

    iter                 end           (void)       = delete;
    iter                 end           (void) const = delete;
    citer                cend          (void)       = delete;

    riter                rbegin        (void)       = delete;
    criter               rbegin        (void) const = delete;
    criter               crbegin       (void)       = delete;

    ref&                 front         (void)       = delete;
    cref&                front         (void) const = delete;

    ref&                 back          (void)       = delete;
    cref&                back          (void) const = delete;

    subrange_view<iter>  view          (void)       = delete;
    subrange_view<citer> view          (void) const = delete;

    iter                 advance_begin (diff_t)     = delete;
    iter                 advance_end   (diff_t)     = delete;

  protected:
    template <typename U>
    struct is_nothrow_swappable
    {
      static constexpr
      bool
      test (void)
      {
        using std::swap;
        return noexcept (swap (std::declval<U&> (), std::declval<U&> ()));
      }

      static constexpr bool value = test ();
    };

    void
    partition_swap (partition_subrange& other)
      noexcept (is_nothrow_swappable<container_type>::value)
    {
      using std::swap;
      swap (m_container, other.m_container);
    }

  private:
    static void propagate_first_right (citer, iter) noexcept { }
  };

  // holds the container
  template <typename T, std::size_t N, typename Container>
  class partition_subrange<list_partition<T, N, Container>, partition_base_index>
  {
    template <typename, std::size_t, typename>
    friend class partition_subrange;

  public:
    using partition_type = list_partition<T, N, Container>;
    using subrange_type  = partition_subrange<partition_type, partition_base_index>;
//  using prev_type      = void;
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

  protected:
    partition_subrange            (void)                          = default;
    partition_subrange            (const partition_subrange&)     = default;
    partition_subrange            (partition_subrange&&) noexcept = default;
    partition_subrange& operator= (const partition_subrange&)     = default;
    partition_subrange& operator= (partition_subrange&&) noexcept = default;
    ~partition_subrange           (void)                          = default;

    template <std::size_t M>
    partition_subrange (const partition_subrange<list_partition<T, M, Container>,
                                                 partition_base_index>& other)
      : m_container (other.m_container)
    { }

    template <std::size_t M>
    partition_subrange (partition_subrange<list_partition<T, M, Container>,
                                           partition_base_index>&& other)
      : m_container (std::move (other.m_container))
    { }

    explicit
    partition_subrange (const alloc_t& alloc)
      : m_container (alloc)
    { }

  public:
    alloc_t
    get_allocator (void) const noexcept
    {
      return m_container.get_allocator ();
    }

    iter   begin   (void)       noexcept { return m_container.begin ();   }
    citer  begin   (void) const noexcept { return m_container.begin ();   }
    citer  cbegin  (void) const noexcept { return m_container.cbegin ();  }

    iter   end     (void)       noexcept { return m_container.end ();     }
    citer  end     (void) const noexcept { return m_container.end ();     }
    citer  cend    (void) const noexcept { return m_container.cend ();    }

    riter  rbegin  (void)       noexcept { return m_container.rbegin ();  }
    criter rbegin  (void) const noexcept { return m_container.rbegin ();  }
    criter crbegin (void) const noexcept { return m_container.crbegin (); }

    riter  rend    (void)       noexcept { return m_container.rend ();    }
    criter rend    (void) const noexcept { return m_container.rend ();    }
    criter crend   (void) const noexcept { return m_container.crend ();   }

    ref    front   (void)       noexcept { return m_container.front ();   }
    cref   front   (void) const noexcept { return m_container.front ();   }

    ref    back    (void)       noexcept { return m_container.back ();    }
    cref   back    (void) const noexcept { return m_container.back ();    }

    size_t size    (void) const noexcept { return m_container.size ();    }

    GCH_NODISCARD
    bool
    empty (void) const noexcept
    {
      return m_container.empty ();
    }

    GCH_NODISCARD
    size_t
    max_size (void) const noexcept
    {
      return m_container.max_size ();
    }

    subrange_view<iter>
    view (void)
    {
      return { begin (), end () };
    }

    subrange_view<citer>
    view (void) const
    {
      return { begin (), end () };
    }

  protected:
    template <typename U>
    struct is_nothrow_swappable
    {
      static constexpr
      bool
      test (void)
      {
        using std::swap;
        return noexcept (swap (std::declval<U&> (), std::declval<U&> ()));
      }

      static constexpr bool value = test ();
    };

    void
    partition_swap (partition_subrange& other)
      noexcept (is_nothrow_swappable<container_type>::value)
    {
      using std::swap;
      swap (m_container, other.m_container);
    }

  protected:
    container_type m_container;
  };

  template <typename T, std::size_t N, typename C, std::size_t I>
  inline
  void
  swap (partition_subrange<list_partition<T, N, C>, I>& lhs,
             partition_subrange<list_partition<T, N, C>, I>& rhs)
    noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  inline
  void
  swap (partition_subrange<list_partition<T, N, C>, I>& lhs,
             partition_subrange<list_partition<T, M, C>, J>& rhs)
    noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }

  template <typename T, std::size_t N, typename C, std::size_t I, typename U>
  inline
  typename partition_subrange<list_partition<T, N, C>, I>::size_type
  erase (partition_subrange<list_partition<T, N, C>, I>& c, const U& val)
  {
    return c.remove (val);
  }

  template <typename T, std::size_t N, typename C, std::size_t I, typename Pred>
  inline
  typename partition_subrange<list_partition<T, N, C>, I>::size_type
  erase_if (partition_subrange<list_partition<T, N, C>, I>& c, Pred pred)
  {
    return c.remove_if (pred);
  }

  template <typename T, std::size_t N, typename Container>
  class list_partition
    : public partition_traits<list_partition<T, N, Container>>,
      protected partition_subrange<list_partition<T, N, Container>, 0>
  {
  public:
    using traits = partition_traits<list_partition>;

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

    using first_type = partition_subrange<list_partition, 0>;
    using last_type  = partition_subrange<list_partition, N>;

    template <std::size_t Index>
    using subrange_type = partition_subrange<list_partition, Index>;

  protected:
    using last_type::m_container;

  public:

    list_partition            (void)                      = default;
    list_partition            (const list_partition&)     = default;
    list_partition            (list_partition&&) noexcept = default;
    list_partition& operator= (const list_partition&)     = default;
    list_partition& operator= (list_partition&&) noexcept = default;
    ~list_partition           (void)                      = default;

    // concatenation constructor
    template <std::size_t M, typename ...Partitions>
    constexpr
    list_partition (const list_partition<T, M, Container>& p, Partitions&&... ps)
      : first_type (get_subrange<0> (p), std::forward<Partitions> (ps)...)
    { }

    template <std::size_t M, typename ...Partitions>
    constexpr
    list_partition (list_partition<T, M, Container>&& p, Partitions&&... ps)
      : first_type (get_subrange<0> (p), std::forward<Partitions> (ps)...)
    { }

    constexpr explicit
    list_partition (const data_alloc_t& alloc)
      : first_type (alloc)
    { }

    template <std::size_t I, typename PartitionRef>
    friend constexpr
    get_subrange_t<I, PartitionRef>
    get_subrange (PartitionRef&& p) noexcept;

    template <typename SubrangeRef>
    friend constexpr
    get_partition_t<SubrangeRef>
    get_partition (SubrangeRef&& s) noexcept;

    GCH_CPP14_CONSTEXPR
    subrange_type<0>&
    front (void) noexcept
    {
      return get_subrange<0> (*this);
    }

    constexpr
    const subrange_type<0>&
    front (void) const noexcept
    {
      return get_subrange<0> (*this);
    }

    GCH_CPP14_CONSTEXPR
    subrange_type<N - 1>&
    back (void) noexcept
    {
      return get_subrange<N - 1> (*this);
    }

    constexpr
    const subrange_type<N - 1>&
    back (void) const noexcept
    {
      return get_subrange<N - 1> (*this);
    }

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

    data_size_t
    data_size (void) const noexcept
    {
      return m_container.size ();
    }

    GCH_NODISCARD
    bool
    data_empty (void) const noexcept
    {
      return m_container.empty ();
    }

    subrange_view<data_iter>
    get_data_view (void)
    {
      return { m_container.begin (), m_container.end () };
    }

    subrange_view<data_citer>
    get_data_view (void) const
    {
      return { m_container.begin (), m_container.end () };
    }

    gch::partition_view<list_partition, N>
    get_partition_view (void)
    {
      return gch::partition_view<list_partition, N> (*this);
    }

    gch::partition_view<const list_partition, N>
    get_partition_view (void) const
    {
      return gch::partition_view<const list_partition, N> (*this);
    }

    template <std::size_t Idx>
    gch::subrange_view<data_iter>
    get_subrange_view (void)
    {
      return get_subrange<Idx> (*this).view ();
    }

    template <std::size_t Idx>
    gch::subrange_view<data_citer>
    get_subrange_view (void) const
    {
      return get_subrange<Idx> (*this).view ();
    }

    template <std::size_t Index,
              typename = typename std::enable_if<(0 < Index) && (Index < N)>::type>
    data_iter
    advance_begin (data_diff_t change)
    {
      return get_subrange<Index> (*this).advance_begin (change);
    }

    template <std::size_t Index,
              typename = typename std::enable_if<(Index < N)>::type>
    data_iter
    advance_end (data_diff_t change)
    {
      return get_subrange<Index> (*this).advance_end (change);
    }

    void
    swap (list_partition& other)
      noexcept (noexcept (std::declval<list_partition&> ().partition_swap (other)))
    {
      first_type::partition_swap (other);
    }

#ifdef GCH_PARTITION_ITERATOR

    using iter   = partition_iterator<list_partition>;
    using citer  = partition_iterator<const list_partition>;
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

    static constexpr
    std::size_t
    size (void) noexcept
    {
      return N;
    }

    template <typename ...Fs>
    static constexpr
    auto
    overload (Fs&&... fs) noexcept
    {
      return partition_overloader<list_partition, Fs...> (std::forward<Fs> (fs)...);
    }

#endif
  };

  template <typename T, std::size_t N, typename Container>
  inline
  void
  swap (list_partition<T, N, Container>& lhs, list_partition<T, N, Container>& rhs)
  {
    lhs.swap (rhs);
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  GCH_ALG_CONSTEXPR inline
  bool
  operator== (const partition_subrange<list_partition<T, N, C>, I>& lhs,
              const partition_subrange<list_partition<T, M, C>, J>& rhs)
  {
    using citer = typename partition_subrange<list_partition<T, N, C>, I>::const_iterator;

    citer it_lhs  = lhs.begin ();
    citer it_rhs  = rhs.begin ();

    citer end_lhs = lhs.end ();
    citer end_rhs = rhs.end ();

    while (it_lhs != end_lhs && it_rhs != end_rhs && *it_lhs == *it_rhs)
    {
      std::advance (it_lhs, 1);
      std::advance (it_rhs, 1);
    }
    return it_lhs == end_lhs && it_rhs == end_rhs;
  }

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  constexpr
  auto
  operator<=> (const partition_subrange<list_partition<T, N, C>, I>& lhs,
               const partition_subrange<list_partition<T, M, C>, J>& rhs)
    requires std::three_way_comparable<T>
  {
    return std::lexicographical_compare_three_way (lhs.begin (), lhs.end (),
                                                   rhs.begin (), rhs.end (),
                                                   std::compare_three_way { });
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  constexpr
  auto
  operator<=> (const partition_subrange<list_partition<T, N, C>, I>& lhs,
               const partition_subrange<list_partition<T, M, C>, J>& rhs)
    requires (! std::three_way_comparable<T>)
  {
    constexpr auto comparison = [](const T& l, const T& r)
                                {
                                  return (l < r) ? std::weak_ordering::less
                                                 : (r < l) ? std::weak_ordering::greater
                                                           : std::weak_ordering::equivalent;
                                };
    return std::lexicographical_compare_three_way (lhs.begin (), lhs.end (),
                                                   rhs.begin (), rhs.end (),
                                                   comparison);
  }

#else

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  inline
  bool
  operator!= (const partition_subrange<list_partition<T, N, C>, I>& lhs,
              const partition_subrange<list_partition<T, M, C>, J>& rhs)
  {
    return ! (lhs == rhs);
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  inline
  bool
  operator< (const partition_subrange<list_partition<T, N, C>, I>& lhs,
             const partition_subrange<list_partition<T, M, C>, J>& rhs)
  {
    return std::lexicographical_compare (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  inline
  bool
  operator<= (const partition_subrange<list_partition<T, N, C>, I>& lhs,
              const partition_subrange<list_partition<T, M, C>, J>& rhs)
  {
    return ! (lhs > rhs);
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  inline
  bool
  operator> (const partition_subrange<list_partition<T, N, C>, I>& lhs,
             const partition_subrange<list_partition<T, M, C>, J>& rhs)
  {
    return rhs < lhs;
  }

  template <typename T, typename C, std::size_t N, std::size_t I, std::size_t M, std::size_t J>
  inline
  bool
  operator>= (const partition_subrange<list_partition<T, N, C>, I>& lhs,
              const partition_subrange<list_partition<T, M, C>, J>& rhs)
  {
    return ! (lhs < rhs);
  }

#endif

  template <typename Container, std::size_t N>
  struct as_list_partition
  {
    using type = list_partition<typename Container::value_type, N, Container>;
  };

  template <typename Container, std::size_t N>
  using as_list_partition_t = typename as_list_partition<Container, N>::type;

}

namespace std
{

  template <typename T, std::size_t N, typename Container>
  struct tuple_size<gch::list_partition<T, N, Container>>
    : public gch::partition_size<gch::list_partition<T, N, Container>>
  { };

  template <std::size_t Index, typename T, std::size_t N, typename Container>
  struct tuple_element<Index, gch::list_partition<T, N, Container>>
    : public gch::partition_element<Index, gch::list_partition<T, N, Container>>
  { };

}

#endif // PARTITION_LIST_PARTITION_HPP
