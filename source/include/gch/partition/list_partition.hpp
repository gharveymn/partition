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
  template <typename Container, std::size_t N, std::size_t Index, typename Enable = void>
  class list_partition_subrange;
  
  template <typename T, std::size_t N, typename Container = std::list<T>>
  class list_partition;
  
  template <typename Container, std::size_t N>
  class list_partition_subrange<Container, N, 0>
    : public list_partition_subrange<Container, N, 1>
  {
    template <typename, std::size_t, std::size_t, typename>
    friend class list_partition_subrange;
    
  public:
    using container_type = Container;
    using subrange_type  = list_partition_subrange<container_type, N, 0>;
    using prev_type      = void;
    using next_type      = list_partition_subrange<container_type, N, 1>;
    
    static constexpr std::size_t index (void) noexcept { return 0; }
    
    GCH_CPP14_CONSTEXPR next_type& next (void) noexcept
    {
      return static_cast<next_type&> (this);
    }
  
    constexpr const next_type& next (void) const noexcept
    {
      return static_cast<next_type&> (this);
    }
    
    void prev (void) = delete;

  protected:
    using next_type::m_container;

  public:
    using iter       = typename container_type::iterator;
    using citer      = typename container_type::const_iterator;
    using riter      = typename container_type::reverse_iterator;
    using criter     = typename container_type::const_reverse_iterator;
    using ref        = typename container_type::reference;
    using cref       = typename container_type::const_reference;
    using size_type  = typename container_type::size_type;
    using value_type = typename container_type::value_type;
    using alloc_type = typename container_type::value_type;
  
    using next_type::num_subranges;
  
//  list_partition_subrange            (void)                               = impl;
//  list_partition_subrange            (const list_partition_subrange&)     = impl;
//  list_partition_subrange            (list_partition_subrange&&) noexcept = impl;
//  list_partition_subrange& operator= (const list_partition_subrange&)     = impl;
//  list_partition_subrange& operator= (list_partition_subrange&&) noexcept = impl;
    ~list_partition_subrange           (void)                               = default;
    
  protected:
    list_partition_subrange (void)
      : next_type ()
    { }
  
    list_partition_subrange (const list_partition_subrange& other)
      : next_type (other)
    { }
  
    list_partition_subrange (list_partition_subrange&& other)
      : next_type (std::move (other))
    { }
  
    template <std::size_t M, typename ...Subranges>
    list_partition_subrange (const list_partition_subrange<Container, M, 0>& other,
                             Subranges&&... subranges)
      : next_type (static_cast<const list_partition_subrange<Container, M, 1>&> (other),
                   std::forward<Subranges> (subranges)...)
    { }
  
    template <std::size_t M, typename ...Subranges>
    list_partition_subrange (list_partition_subrange<Container, M, 0>&& other,
                             Subranges&&... subranges)
      : next_type (static_cast<list_partition_subrange<Container, M, 1>&&> (other),
                   std::forward<Subranges> (subranges)...)
    { }
  
    template <typename T, std::size_t M, typename ...Subranges>
    list_partition_subrange (list_partition<T, M, Container>&& partition,
                             Subranges&&... subranges)
      : list_partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }
  
    template <typename T, std::size_t M, typename ...Subranges>
    list_partition_subrange (const list_partition<T, M, Container>& partition,
                             Subranges&&... subranges)
      : list_partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }
  
  public:
    list_partition_subrange& operator= (const list_partition_subrange& other) noexcept
    {
      if (&other != this)
      {
        iter it1 = begin ();
        iter it2 = other.begin ();
        for (; it1 != end () && it2 != other.end (); ++it1, ++it2)
          *it1 = *it2;
        erase (it1, end ());
        insert (it2, other.end ());
      }
      return *this;
    }
  
    list_partition_subrange& operator= (list_partition_subrange&& other) noexcept
    {
      if (empty ())
      {
        splice (cend (), std::move (other));
      }
      else
      {
        iter last = --end ();
        splice (cend (), std::move (other));
        erase (begin (), last);
      }
      return *this;
    }
  
    void assign (size_type count, const value_type& val)
    {
      iter it = begin ();
      for (; it != end () && count != 0; ++it, --count)
        *it = val;
    
      if (count > 0)
        insert (end (), count, val);
      else
        erase (count, end ());
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
  
    void assign (std::initializer_list<value_type> ilist)
    {
      assign (ilist.begin (), ilist.end ());
    }
  
    explicit list_partition_subrange (iter)
    { }
  
    GCH_NODISCARD iter  begin  (void)       noexcept { return m_container.begin ();               }
    GCH_NODISCARD citer begin  (void) const noexcept { return m_container.cbegin ();              }
    GCH_NODISCARD citer cbegin (void) const noexcept { return m_container.cbegin ();              }
  
    GCH_NODISCARD iter   end     (void)       noexcept { return next_type::begin ();              }
    GCH_NODISCARD citer  end     (void) const noexcept { return next_type::cbegin ();             }
    GCH_NODISCARD citer  cend    (void) const noexcept { return next_type::cbegin ();             }
  
    GCH_NODISCARD riter  rbegin  (void)       noexcept { return riter (end ());                   }
    GCH_NODISCARD criter rbegin  (void) const noexcept { return criter (cend ());                 }
    GCH_NODISCARD criter crbegin (void) const noexcept { return criter (cend ());                 }
  
    GCH_NODISCARD riter  rend    (void)       noexcept { return riter (begin ());                 }
    GCH_NODISCARD criter rend    (void) const noexcept { return criter (cbegin ());               }
    GCH_NODISCARD criter crend   (void) const noexcept { return criter (cbegin ());               }
  
    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   front   (void)       noexcept { return *begin ();    }
    GCH_NODISCARD constexpr           cref&  front   (void) const noexcept { return *begin ();    }
  
    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   back    (void)       noexcept { return *(--end ());  }
    GCH_NODISCARD constexpr           cref&  back    (void) const noexcept { return *(--cend ()); }
  
    GCH_NODISCARD std::size_t size (void) const noexcept
    {
      return std::distance (cbegin (), cend ());
    }
  
    GCH_NODISCARD bool empty (void) const noexcept
    {
      return cbegin () == cend ();
    }
  
    void clear (void) noexcept
    {
      m_container.erase (cbegin (), cend ());
    }
  
    template <typename ...Args>
    iter insert (const citer pos, Args&&... args)
    {
      return m_container.insert (pos, std::forward<Args> (args)...);
    }
  
    template <typename ...Args>
    iter emplace (const citer pos, Args&&... args)
    {
      return m_container.emplace (pos, std::forward<Args> (args)...);
    }
  
    iter erase (const citer pos)
    {
      return m_container.erase (pos);
    }
  
    iter erase (const citer first, const citer last)
    {
      return m_container.erase (first, last);
    }
  
    template <typename T>
    void push_back (T&& val)
    {
      m_container.insert (end (), std::forward<T> (val));
    }
  
    template <typename ...Args>
    ref emplace_back (Args&&... args)
    {
      return *m_container.emplace (end (), std::forward<Args> (args)...);
    }
  
    void pop_back (void)
    {
      erase (--cend ());
    }
  
    template <typename T>
    void push_front (T&& val)
    {
      m_container.push_front (std::forward<T> (val));
    }
  
    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      return *m_container.emplace (begin (), std::forward<Args> (args)...);
    }
  
    void pop_front (void)
    {
      m_container.pop_front ();
    }
  
    template <std::size_t M, std::size_t Idx>
    void swap (list_partition_subrange<container_type, M, Idx>& other)
      noexcept (noexcept (std::declval<container_type> ().swap (std::declval<container_type&> ())))
    {
      if (&other == this)
        return;
      
      container_type tmp1;
      container_type tmp2;
      tmp1.splice (tmp1.cend (), std::move (m_container), cbegin (), cend ());
      tmp2.splice (tmp2.cend (), std::move (other.m_container), other.cbegin (), other.cend ());
      tmp1.swap (tmp2);
  
      iter new_first2 = tmp2.begin ();
    
      m_container.splice (cend (), tmp1);
    
      other.m_container.splice (cend (), tmp2);
      other.absolute_propagate_first (new_first2);
    }
  
  
    template <std::size_t M, std::size_t Idx, typename ...Args>
    void merge (list_partition_subrange<container_type, M, Idx>& other, Args&&... args)
    {
      merge (std::move (other), std::forward<Args> (args)...);
    }
  
    template <std::size_t M, std::size_t Idx, typename ...Args>
    void merge (list_partition_subrange<container_type, M, Idx>&& other, Args&&... args)
    {
      if (&other == this)
        return;
      
      container_type tmp1;
      container_type tmp2;
      tmp1.splice (tmp1.cend (), std::move (m_container), cbegin (), cend ());
      tmp2.splice (tmp2.cend (), std::move (other.m_container), other.cbegin (), other.cend ());
      tmp1.merge (std::move (tmp2), std::forward<Args> (args)...);
      m_container.splice (cend (), tmp1);
      other.absolute_propagate_first (other.end ());
    }
  
    template <std::size_t M, std::size_t Idx, typename ...Args>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>& other, Args&&... args)
    {
      splice (pos, std::move (other), std::forward<Args> (args)...);
    }
  
    template <std::size_t M, std::size_t Idx>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>&& other)
    {
      m_container.splice (pos, std::move (other.m_container), other.cbegin (), other.cend ());
      other.absolute_propagate_first (other.cend ());
    }
  
    template <std::size_t M, std::size_t Idx>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>&& other, citer it)
    {
      iter new_other_front = std::next (it);
      m_container.splice (pos, std::move (other.m_container), it);
      other.propagate_first (it, new_other_front);
    }
  
    template <std::size_t M, std::size_t Idx>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>&& other,
                 citer first, citer last)
    {
      m_container.splice (pos, std::move (other.m_container), first, last);
      other.propagate_first (first, last);
    }
  
    size_type remove (const value_type& val)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      
      size_type original_size = tmp.size ();
      tmp.remove (val);
      size_type num_removed = original_size - tmp.size ();
      
      m_container.splice (cend (), tmp);
      
      return num_removed;
    }
  
    template <typename UnaryPredicate>
    size_type remove_if (UnaryPredicate p)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
  
      size_type original_size = tmp.size ();
      tmp.remove_if (p);
      size_type num_removed = original_size - tmp.size ();
      
      m_container.splice (cend (), tmp);
      
      return num_removed;
    }
  
    template <typename ...Args>
    void unique (Args&&... args)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      tmp.unique (std::forward<Args> (args)...);
      m_container.splice (cend (), tmp);
    }
  
    template <typename ...Args>
    void sort (Args&&... args)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      tmp.sort (std::forward<Args> (args)...);
      m_container.splice (cend (), tmp);
    }
  
    void resize (size_type count)
    {
      citer cit = resize_pos (count);
      if (count)
      {
        size_type idx = 0;
        try
        {
          for (; idx < count; ++idx)
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
        erase (cit, cend ());
    }
  
    void resize (size_type count, const value_type& val)
    {
      citer cit = resize_pos (count);
      if (count)
        insert (cend (), count, val);
      else
        erase (cit, cend ());
    }
  
    subrange_view<iter> view (void)
    {
      return { begin (), end () };
    }
  
    subrange_view<citer> view (void) const
    {
      return { begin (), end () };
    }

  private:
  
    void absolute_propagate_first (iter) { }
    void propagate_first (iter, iter) { }
  
    citer resize_pos (size_type& count) const
    {
      citer cit;
      const size_type len = subrange_cast (this)->size ();
      if (count < len)
      {
        if (count <= len / 2)
        {
          cit = cbegin ();
          std::advance (cit, count);
        }
        else
        {
          cit = cend ();
          std::ptrdiff_t num_erase = len - count;
          std::advance (cit, -num_erase);
        }
        count = 0;
        return cit;
      }
      cit = cend ();
      count -= len;
      return cit;
    }
  
  };
  
  template <typename Container, std::size_t N, std::size_t Index>
  class list_partition_subrange<Container, N, Index,
                                typename std::enable_if<(0 < Index) && (Index < N)>::type>
    : public list_partition_subrange<Container, N, Index + 1>
  {
    template <typename, std::size_t, std::size_t, typename>
    friend class list_partition_subrange;
    
  public:
    using container_type = Container;
    using subrange_type  = list_partition_subrange<container_type, N, Index>;
    using prev_type      = list_partition_subrange<container_type, N, Index - 1>;
    using next_type      = list_partition_subrange<container_type, N, Index + 1>;
  
    static constexpr std::size_t index (void) noexcept { return Index; }
  
    GCH_CPP14_CONSTEXPR next_type& next (void) noexcept
    {
      return static_cast<next_type&> (this);
    }
  
    constexpr const next_type& next (void) const noexcept
    {
      return static_cast<next_type&> (this);
    }
  
    GCH_CPP14_CONSTEXPR prev_type& prev (void) noexcept
    {
      return static_cast<prev_type&> (this);
    }
  
    constexpr const prev_type& prev (void) const noexcept
    {
      return static_cast<prev_type&> (this);
    }
    
  protected:
    using next_type::m_container;
  
  public:
    using iter       = typename container_type::iterator;
    using citer      = typename container_type::const_iterator;
    using riter      = typename container_type::reverse_iterator;
    using criter     = typename container_type::const_reverse_iterator;
    using ref        = typename container_type::reference;
    using cref       = typename container_type::const_reference;
    using size_type  = typename container_type::size_type;
    using value_type = typename container_type::value_type;
    using alloc_type = typename container_type::value_type;
    
    using next_type::num_subranges;
    
//  list_partition_subrange            (void)                               = impl;
//  list_partition_subrange            (const list_partition_subrange&)     = impl;
//  list_partition_subrange            (list_partition_subrange&&) noexcept = impl;
//  list_partition_subrange& operator= (const list_partition_subrange&)     = impl;
//  list_partition_subrange& operator= (list_partition_subrange&&) noexcept = impl;
    ~list_partition_subrange           (void)                               = default;

  protected:
    list_partition_subrange (void)
      : next_type (),
        m_first (m_container.end ())
    { }
  
    list_partition_subrange (const list_partition_subrange& other)
      : next_type (other),
        m_first (std::next (m_container.begin (),
                            std::distance (other.m_container.begin (), other.begin ())))
    { }
  
    list_partition_subrange (list_partition_subrange&& other)
      : next_type (std::move (other)),
        m_first (other.m_first)
    { }
  
    template <std::size_t M, std::size_t OtherIdx, typename ...Subranges,
      typename std::enable_if<(OtherIdx < M)>::type * = nullptr>
    list_partition_subrange (const list_partition_subrange<Container, M, OtherIdx>& other,
                             Subranges&&... subranges)
      : next_type (static_cast<const list_partition_subrange<Container, M, OtherIdx + 1>&> (other),
                   std::forward<Subranges> (subranges)...),
        m_first (std::next (m_container.begin (),
                            std::distance (other.m_container.begin (), other.begin ())))
    { }
  
    template <std::size_t M, std::size_t OtherIdx, typename ...Subranges,
      typename std::enable_if<(OtherIdx == M)>::type * = nullptr>
    list_partition_subrange (const list_partition_subrange<Container, M, OtherIdx>& other,
                             Subranges&&... subranges)
      : list_partition_subrange (std::forward<Subranges> (subranges)...)
    {
      m_container.insert (m_container.cbegin (), other.m_container.cbegin (),
                          other.m_container.cend ());
    }
  
    template <std::size_t M, std::size_t OtherIdx, typename ...Subranges,
              typename std::enable_if<(OtherIdx < M)>::type * = nullptr>
    list_partition_subrange (list_partition_subrange<Container, M, OtherIdx>&& other,
                             Subranges&&... subranges)
      : next_type (static_cast<list_partition_subrange<Container, M, OtherIdx + 1>&&> (other),
                   std::forward<Subranges> (subranges)...),
        m_first (other.begin ())
    { }
  
    template <std::size_t M, std::size_t OtherIdx, typename ...Subranges,
              typename std::enable_if<(OtherIdx == M)>::type * = nullptr>
    list_partition_subrange (list_partition_subrange<Container, M, OtherIdx>&& other,
                             Subranges&&... subranges)
      : list_partition_subrange (std::forward<Subranges> (subranges)...)
    {
      m_container.splice (m_container.cbegin (), other.m_container);
    }
  
    template <typename T, std::size_t M, typename ...Subranges>
    list_partition_subrange (list_partition<T, M, Container>&& partition,
                             Subranges&&... subranges)
      : list_partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }
  
    template <typename T, std::size_t M, typename ...Subranges>
    list_partition_subrange (const list_partition<T, M, Container>& partition,
                             Subranges&&... subranges)
      : list_partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }
  
  public:
    list_partition_subrange& operator= (const list_partition_subrange& other) noexcept
    {
      if (&other != this)
      {
        iter it1 = begin ();
        iter it2 = other.begin ();
        for (; it1 != end () && it2 != other.end (); ++it1, ++it2)
          *it1 = *it2;
        erase (it1, end ());
        insert (it2, other.end ());
      }
      return *this;
    }
  
    list_partition_subrange& operator= (list_partition_subrange&& other) noexcept
    {
      if (empty ())
      {
        splice (cend (), std::move (other));
      }
      else
      {
        iter last = --end ();
        splice (cend (), std::move (other));
        erase (begin (), last);
      }
      return *this;
    }
    
    void assign (size_type count, const value_type& val)
    {
      iter it = begin ();
      for (; it != end () && count != 0; ++it, --count)
        *it = val;
      
      if (count > 0)
        insert (end (), count, val);
      else
        erase (count, end ());
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
  
    void assign (std::initializer_list<value_type> ilist)
    {
      assign (ilist.begin (), ilist.end ());
    }
  
    GCH_NODISCARD iter  begin  (void)       noexcept { return m_first;                            }
    GCH_NODISCARD citer begin  (void) const noexcept { return citer (m_first);                    }
    GCH_NODISCARD citer cbegin (void) const noexcept { return citer (m_first);                    }
  
    GCH_NODISCARD iter   end     (void)       noexcept { return next_type::begin ();              }
    GCH_NODISCARD citer  end     (void) const noexcept { return next_type::cbegin ();             }
    GCH_NODISCARD citer  cend    (void) const noexcept { return next_type::cbegin ();             }
  
    GCH_NODISCARD riter  rbegin  (void)       noexcept { return riter (end ());                   }
    GCH_NODISCARD criter rbegin  (void) const noexcept { return criter (cend ());                 }
    GCH_NODISCARD criter crbegin (void) const noexcept { return criter (cend ());                 }
  
    GCH_NODISCARD riter  rend    (void)       noexcept { return riter (begin ());                 }
    GCH_NODISCARD criter rend    (void) const noexcept { return criter (cbegin ());               }
    GCH_NODISCARD criter crend   (void) const noexcept { return criter (cbegin ());               }
  
    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   front   (void)       noexcept { return *begin ();    }
    GCH_NODISCARD constexpr           cref&  front   (void) const noexcept { return *begin ();    }
  
    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   back    (void)       noexcept { return *(--end ());  }
    GCH_NODISCARD constexpr           cref&  back    (void) const noexcept { return *(--cend ()); }
  
    GCH_NODISCARD std::size_t size (void) const noexcept
    {
      return std::distance (cbegin (), cend ());
    }
  
    GCH_NODISCARD bool empty (void) const noexcept
    {
      return cbegin () == cend ();
    }
    
    void clear (void) noexcept
    {
      absolute_propagate_first (m_container.erase (cbegin (), cend ()));
    }
    
    template <typename ...Args>
    iter insert (const citer pos, Args&&... args)
    {
      iter ret = m_container.insert (pos, std::forward<Args> (args)...);
      propagate_first (pos, ret);
      return ret;
    }
    
    template <typename ...Args>
    iter emplace (const citer pos, Args&&... args)
    {
      iter ret = m_container.emplace (pos, std::forward<Args> (args)...);
      propagate_first (pos, ret);
      return ret;
    }
  
    iter erase (const citer pos)
    {
      iter ret = m_container.erase (pos);
      propagate_first (pos, ret);
      return ret;
    }
  
    iter erase (const citer first, const citer last)
    {
      iter ret = m_container.erase (first, last);
      propagate_first (first, ret);
      return ret;
    }
    
    template <typename T>
    void push_back (T&& val)
    {
      iter it = m_container.insert (cend (), std::forward<T> (val));
      propagate_first (cend (), it);
    }
  
    template <typename ...Args>
    ref emplace_back (Args&&... args)
    {
      iter ret = m_container.emplace (cend (), std::forward<Args> (args)...);
      propagate_first (end (), ret);
      return *ret;
    }
    
    void pop_back (void)
    {
      erase (--cend ());
    }
    
    template <typename T>
    void push_front (T&& val)
    {
      absolute_propagate_first (m_container.insert (m_first, std::forward<T> (val)));
    }
  
    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      absolute_propagate_first (m_container.emplace (m_first, std::forward<Args> (args)...));
      return *m_first;
    }
  
    void pop_front (void)
    {
      absolute_propagate_first (m_container.erase (m_first));
    }
    
    void resize (size_type count)
    {
      citer cit = resize_pos (count);
      if (count)
      {
        size_type idx = 0;
        try
        {
          for (; idx < count; ++idx)
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
        erase (cit, cend ());
    }
  
    void resize (size_type count, const value_type& val)
    {
      citer cit = resize_pos (count);
      if (count)
        insert (cend (), count, val);
      else
        erase (cit, cend ());
    }
    
    template <std::size_t M, std::size_t Idx>
    void swap (list_partition_subrange<container_type, M, Idx>& other)
      noexcept (noexcept (std::declval<container_type> ().swap (std::declval<container_type&> ())))
    {
      if (&other == this)
        return;
      
      container_type tmp1;
      container_type tmp2;
      tmp1.splice (tmp1.cend (), std::move (m_container), cbegin (), cend ());
      tmp2.splice (tmp2.cend (), std::move (other.m_container), other.cbegin (), other.cend ());
      tmp1.swap (tmp2);
      
      iter new_first1 = tmp1.begin ();
      iter new_first2 = tmp2.begin ();
      
      m_container.splice (cend (), tmp1);
      absolute_propagate_first (new_first1);
      
      other.m_container.splice (cend (), tmp2);
      other.absolute_propagate_first (new_first2);
    }
    

    template <std::size_t M, std::size_t Idx, typename ...Args>
    void merge (list_partition_subrange<container_type, M, Idx>& other, Args&&... args)
    {
      merge (std::move (other), std::forward<Args> (args)...);
    }
  
    template <std::size_t M, std::size_t Idx, typename ...Args>
    void merge (list_partition_subrange<container_type, M, Idx>&& other, Args&&... args)
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
      absolute_propagate_first (new_first);
      other.absolute_propagate_first (other.end ());
    }
  
    template <std::size_t M, std::size_t Idx, typename ...Args>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>& other, Args&&... args)
    {
      splice (pos, std::move (other), std::forward<Args> (args)...);
    }
  
    template <std::size_t M, std::size_t Idx>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>&& other)
    {
      citer first = other.cbegin ();
      m_container.splice (pos, std::move (other.m_container), first, other.cend ());
      propagate_first (pos, first);
      other.absolute_propagate_first (other.cend ());
    }
  
    template <std::size_t M, std::size_t Idx>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>&& other, citer cit)
    {
      citer new_other_front = std::next (cit);
      m_container.splice (pos, std::move (other.m_container), cit);
      propagate_first (pos, cit);
      other.propagate_first (cit, new_other_front);
    }
  
    template <std::size_t M, std::size_t Idx>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>&& other,
                 citer first, citer last)
    {
      m_container.splice (pos, std::move (other.m_container), first, last);
      propagate_first (pos, first);
      other.propagate_first (first, last);
    }
    
    size_type remove (const value_type& val)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
  
      size_type original_size = tmp.size ();
      tmp.remove (val);
      size_type num_removed = original_size - tmp.size ();
      
      iter new_first = tmp.begin ();
      m_container.splice (cend (), tmp);
      absolute_propagate_first (new_first);
      
      return num_removed;
    }
  
    template <typename UnaryPredicate>
    size_type remove_if (UnaryPredicate p)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      
      size_type original_size = tmp.size ();
      tmp.remove_if (p);
      size_type num_removed = original_size - tmp.size ();
      
      iter new_first = tmp.begin ();
      m_container.splice (cend (), tmp);
      absolute_propagate_first (new_first);
      
      return num_removed;
    }
  
    template <typename ...Args>
    void unique (Args&&... args)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      tmp.unique (std::forward<Args> (args)...);
      iter new_first = tmp.begin ();
      m_container.splice (cend (), tmp);
      absolute_propagate_first (new_first);
    }
  
    template <typename ...Args>
    void sort (Args&&... args)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      tmp.sort (std::forward<Args> (args)...);
      iter new_first = tmp.begin ();
      m_container.splice (cend (), tmp);
      absolute_propagate_first (new_first);
    }
  
    subrange_view<iter> view (void)
    {
      return { begin (), end () };
    }
  
    subrange_view<citer> view (void) const
    {
      return { begin (), end () };
    }
    
  private:
  
    void absolute_propagate_first (iter replace)
    {
      static_cast<prev_type *> (this)->propagate_first (m_first, replace);
      m_first = replace;
    }
    
    void propagate_first (iter cmp, iter replace)
    {
      if (m_first == cmp)
      {
        static_cast<prev_type *> (this)->propagate_first (cmp, replace);
        m_first = replace;
      }
    }
    
    citer resize_pos (size_type& count) const
    {
      citer cit;
      const size_type len = subrange_cast (this)->size ();
      if (count < len)
      {
        if (count <= len / 2)
        {
          cit = cbegin ();
          std::advance (cit, count);
        }
        else
        {
          cit = cend ();
          std::ptrdiff_t num_erase = len - count;
          std::advance (cit, -num_erase);
        }
        count = 0;
        return cit;
      }
      cit = cend ();
      count -= len;
      return cit;
    }
    
    iter m_first;
  };
  
  // end case holds the actual container
  template <typename Container, std::size_t N>
  class list_partition_subrange<Container, N, N>
  {
    template <typename, std::size_t, std::size_t, typename>
    friend class list_partition_subrange;
    
  public:
    using container_type = Container;
    using subrange_type  = list_partition_subrange<container_type, N, N>;
    using prev_type      = list_partition_subrange<container_type, N, N - 1>;
    using iter           = typename container_type::iterator;
    using citer          = typename container_type::const_iterator;
    using riter          = typename container_type::reverse_iterator;
    using criter         = typename container_type::const_reverse_iterator;
    using ref            = typename container_type::reference;
    using cref           = typename container_type::const_reference;
    using size_type      = typename container_type::size_type;
    using value_type     = typename container_type::value_type;
    using alloc_type     = typename container_type::value_type;
    
    static constexpr std::size_t index (void) noexcept { return N; }
    static constexpr std::size_t num_subranges (void) noexcept { return N; }
  
    void next (void) = delete;
  
    GCH_CPP14_CONSTEXPR prev_type& prev (void) noexcept
    {
      return static_cast<prev_type&> (this);
    }
  
    constexpr const prev_type& prev (void) const noexcept
    {
      return static_cast<prev_type&> (this);
    }
    
    list_partition_subrange            (void)                               = default;
//  list_partition_subrange            (const list_partition_subrange&)     = impl;
//  list_partition_subrange            (list_partition_subrange&&) noexcept = impl;
    list_partition_subrange& operator= (const list_partition_subrange&)     = default;
    list_partition_subrange& operator= (list_partition_subrange&&) noexcept = default;
    ~list_partition_subrange           (void)                               = default;
  
  protected:
    list_partition_subrange (const list_partition_subrange& other)
      : m_container (other.m_container)
    { }
  
    list_partition_subrange (list_partition_subrange&& other)
      : m_container (std::move (other.m_container))
    { }
  
    template <std::size_t M>
    list_partition_subrange (const list_partition_subrange<Container, M, M>& other)
      : m_container (other.m_container)
    { }
    
    template <std::size_t M>
    list_partition_subrange (list_partition_subrange<Container, M, M>&& other)
      : m_container (std::move (other.m_container))
    { }
    
  public:
    iter  begin  (void)       noexcept { return m_container.end ();  }
    citer begin  (void) const noexcept { return m_container.cend (); }
    citer cbegin (void) const noexcept { return m_container.cend (); }
  
    alloc_type get_allocator (void) const noexcept
    {
      return m_container.get_allocator ();
    }
  
    GCH_NODISCARD size_type max_size (void) const noexcept
    {
      return m_container.max_size ();
    }
  
    subrange_view<iter> view (void)        = delete;
    subrange_view<citer> view (void) const = delete;
  
  protected:
    container_type m_container;
  };
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  void swap (list_partition_subrange<Container, N, Idx1>& lhs,
             list_partition_subrange<Container, M, Idx2>& rhs)
    noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }
  
  template <typename C, std::size_t N, std::size_t Idx, typename T>
  typename list_partition_subrange<C, N, Idx>::size_type
  erase (list_partition_subrange<C, N, Idx>& c, const T& val)
  {
    using cref = typename list_partition_subrange<C, N, Idx>::ref;
    return c.remove_if ([&val] (cref elem) { return elem == val; });
  }
  
  template <typename C, std::size_t N, std::size_t Idx, typename Pred>
  typename list_partition_subrange<C, N, Idx>::size_type
  erase (list_partition_subrange<C, N, Idx>& c, Pred pred)
  {
    return c.remove_if (pred);
  }

  template <typename T, std::size_t N, typename Container>
  class list_partition
    : protected list_partition_subrange<Container, N, 0>
  {
    using base_type = list_partition_subrange<Container, N, 0>;
  public:
    using data_type           = T;
    using container_type      = Container;
    using data_iter           = typename container_type::iterator;
    using data_citer          = typename container_type::const_iterator;
    using data_riter          = typename container_type::reverse_iterator;
    using data_criter         = typename container_type::const_reverse_iterator;
    using data_ref            = typename container_type::reference;
    using data_cref           = typename container_type::const_reference;
    using data_size_type      = typename container_type::size_type;
    using data_value_type     = typename container_type::value_type;
    using data_alloc_type     = typename container_type::value_type;
    
  protected:
    
    using list_partition_subrange<Container, N, 0>::m_container;
    
  public:
    
    list_partition            (void)                      = default;
    list_partition            (const list_partition&)     = default;
    list_partition            (list_partition&&) noexcept = default;
    list_partition& operator= (const list_partition&)     = default;
    list_partition& operator= (list_partition&&) noexcept = default;
    ~list_partition           (void)                      = default;
    
    // concatenation constructor
    template <std::size_t M, typename ...Partitions>
    list_partition (const list_partition<T, M, Container>& p, Partitions&&... ps)
      : base_type (get_subrange<0> (p), std::forward<Partitions> (ps)...)
    { }

    template <std::size_t M, typename ...Partitions>
    list_partition (list_partition<T, M, Container>&& p, Partitions&&... ps)
      : base_type (get_subrange<0> (p), std::forward<Partitions> (ps)...)
    { }
  
    template <std::size_t Index>
    using subrange_type = list_partition_subrange<Container, N, Index>;
  
    template <std::size_t Index, typename Partition>
    friend constexpr partition_element_t<Index, Partition>&
    get_subrange (Partition& p) noexcept;
  
    template <std::size_t Index, typename Partition>
    friend constexpr const partition_element_t<Index, Partition>&
    get_subrange (const Partition& p) noexcept;
  
    template <std::size_t Index, typename Partition>
    friend constexpr partition_element_t<Index, Partition>&&
    get_subrange (Partition&& p) noexcept;
  
    template <std::size_t Index, typename Partition>
    friend constexpr const partition_element_t<Index, Partition>&&
    get_subrange (const Partition&& p) noexcept;
  
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
    
    using base_type::num_subranges;
  
    gch::subrange_view<data_iter> data_view (void)
    {
      return { m_container.begin (), m_container.end () };
    }
  
    gch::subrange_view<data_citer> data_view (void) const
    {
      return { m_container.begin (), m_container.end () };
    }
  
    gch::partition_view<list_partition, N> partition_view (void)
    {
      return gch::partition_view<list_partition, N> (*this);
    }
  
    gch::partition_view<const list_partition, N> partition_view (void) const
    {
      return gch::partition_view<const list_partition, N> (*this);
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
    
    template <typename ...Fs>
    struct overloader
      : partition_overloader<list_partition, Fs...>
    {
      using partition_overloader<list_partition, Fs...>::partition_overloader;
    };
  
    template<typename... Fs> overloader (Fs&&...) -> overloader<Fs...>;
    
    template <typename ...Fs>
    static constexpr auto overload (Fs&&... fs) noexcept
    {
      return overloader<Fs...> (std::forward<Fs> (fs)...);
    }
    
#endif
    
  private:
  };
  
  template <std::size_t Index, typename T, std::size_t N, typename Container>
  constexpr auto get (list_partition<T, N, Container>& p) noexcept
    -> decltype (get_subrange<Index> (p))
  {
    return get_subrange<Index> (p);
  }
  
  template <std::size_t Index, typename T, std::size_t N, typename Container>
  constexpr auto get (const list_partition<T, N, Container>& p) noexcept
    -> decltype (get_subrange<Index> (p))
  {
    return get_subrange<Index> (p);
  }
  
  template <std::size_t Index, typename T, std::size_t N, typename Container>
  constexpr auto get (list_partition<T, N, Container>&& p) noexcept
    -> decltype (get_subrange<Index> (p))
  {
    return get_subrange<Index> (p);
  }
  
  template <std::size_t Index, typename T, std::size_t N, typename Container>
  constexpr auto get (const list_partition<T, N, Container>&& p) noexcept
    -> decltype (get_subrange<Index> (p))
  {
    return get_subrange<Index> (p);
  }
  
  template <typename T, typename Container, typename ...Partitions>
  list_partition<T, total_subranges<Partitions...>::value, Container>
  partition_cat (Partitions&&... ps)
  {
    return { std::forward<Partitions> (ps)... };
  }

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

  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  constexpr bool operator== (const list_partition_subrange<Container, N, Idx1>& lhs,
                             const list_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator<= (const list_partition_subrange<Container, N, Idx1>& lhs,
                    const list_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::lexicographical_compare_three_way (lhs.begin (), lhs.end (),
                                                   rhs.begin (), rhs.end (),
                                                   detail::compare::synth_three_way);
  }

#else
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator== (const list_partition_subrange<Container, N, Idx1>& lhs,
                   const list_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator!= (const list_partition_subrange<Container, N, Idx1>& lhs,
                   const list_partition_subrange<Container, M, Idx2>& rhs)
  {
    return ! (lhs == rhs);
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator< (const list_partition_subrange<Container, N, Idx1>& lhs,
                  const list_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::lexicographical_compare (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator<= (const list_partition_subrange<Container, N, Idx1>& lhs,
                   const list_partition_subrange<Container, M, Idx2>& rhs)
  {
    return ! (lhs > rhs);
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator> (const list_partition_subrange<Container, N, Idx1>& lhs,
                  const list_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::lexicographical_compare (rhs.begin (), rhs.end (), lhs.begin (), lhs.end ());
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator>= (const list_partition_subrange<Container, N, Idx1>& lhs,
                   const list_partition_subrange<Container, M, Idx2>& rhs)
  {
    return ! (lhs < rhs);
  }

#endif
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
