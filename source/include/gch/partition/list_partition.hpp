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

#ifndef GCH_COMPILER_CLANG
#  ifdef __clang__
#    define GCH_COMPILER_CLANG
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
    using container_type          = Container;
  
    using iterator                = typename container_type::iterator;
    using const_iterator          = typename container_type::const_iterator;
    using reverse_iterator        = typename container_type::reverse_iterator;
    using const_reverse_iterator  = typename container_type::const_reverse_iterator;
    using reference               = typename container_type::reference;
    using const_reference         = typename container_type::const_reference;
    using size_type               = typename container_type::size_type;
    using difference_type         = typename container_type::difference_type;
    using value_type              = typename container_type::value_type;
    using allocator_type          = typename container_type::allocator_type;
  
    using iter   = iterator;
    using citer  = const_iterator;
    using riter  = reverse_iterator;
    using criter = const_reverse_iterator;
    using ref    = reference;
    using cref   = const_reference;
    
    using partition_type = list_partition<value_type, N, container_type>;
    using subrange_type  = list_partition_subrange<container_type, N, 0>;
    using prev_type      = void;
    using next_type      = list_partition_subrange<container_type, N, 1>;
    
    static constexpr std::size_t index (void) noexcept { return 0; }

  protected:
    using next_type::m_container;

  public:
    using next_type::num_subranges;
  
//  list_partition_subrange            (void)                               = impl;
//  list_partition_subrange            (const list_partition_subrange&)     = impl;
//  list_partition_subrange            (list_partition_subrange&&) noexcept = impl;
//  list_partition_subrange& operator= (const list_partition_subrange&)     = impl;
//  list_partition_subrange& operator= (list_partition_subrange&&) noexcept = impl;
    ~list_partition_subrange           (void)                               = default;
    
    constexpr list_partition_subrange (void)
      : next_type ()
    { }
  
    constexpr list_partition_subrange (const list_partition_subrange& other)
      : next_type (other)
    { }
  
    constexpr list_partition_subrange (list_partition_subrange&& other)
      : next_type (std::move (other))
    { }

  protected:
    template <std::size_t M, typename ...Subranges>
    constexpr list_partition_subrange (const list_partition_subrange<Container, M, 0>& other,
                                       Subranges&&... subranges)
      : next_type (next_subrange (other), std::forward<Subranges> (subranges)...)
    { }
  
    template <std::size_t M, typename ...Subranges>
    constexpr list_partition_subrange (list_partition_subrange<Container, M, 0>&& other,
                                        Subranges&&... subranges)
      : next_type (next_subrange (other), std::forward<Subranges> (subranges)...)
    { }
  
    template <typename T, std::size_t M, typename ...Subranges>
    constexpr list_partition_subrange (list_partition<T, M, Container>&& partition,
                                       Subranges&&... subranges)
      : list_partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }
  
    template <typename T, std::size_t M, typename ...Subranges>
    constexpr list_partition_subrange (const list_partition<T, M, Container>& partition,
                                       Subranges&&... subranges)
      : list_partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }
  
    constexpr explicit list_partition_subrange (const allocator_type& alloc)
      : next_type (alloc)
    { }
    
  public:
    list_partition_subrange& operator= (const list_partition_subrange& other) noexcept
    {
      if (&other != this)
        assign (other.begin (), other.end ());
      return *this;
    }
  
    list_partition_subrange& operator= (list_partition_subrange&& other) noexcept
    {
      if (! other.empty ())
      {
        iter last = other.begin ();
        splice (cend (), std::move (other));
        erase (begin (), last);
      }
      return *this;
    }
  
    explicit list_partition_subrange (iter) { }
  
    void assign (size_type count, const value_type& val)
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
  
    void assign (std::initializer_list<value_type> ilist)
    {
      assign (ilist.begin (), ilist.end ());
    }
  
    GCH_NODISCARD iter  begin  (void)       noexcept { return m_container.begin ();               }
    GCH_NODISCARD citer begin  (void) const noexcept { return m_container.cbegin ();              }
    GCH_NODISCARD citer cbegin (void) const noexcept { return m_container.cbegin ();              }
  
    GCH_NODISCARD iter   end     (void)       noexcept { return next_type::begin ();              }
    GCH_NODISCARD citer  end     (void) const noexcept { return next_type::cbegin ();             }
    GCH_NODISCARD citer  cend    (void) const noexcept { return next_type::cbegin ();             }
  
    GCH_NODISCARD riter  rbegin  (void)       noexcept { return next_type::rend ();               }
    GCH_NODISCARD criter rbegin  (void) const noexcept { return next_type::crend ();              }
    GCH_NODISCARD criter crbegin (void) const noexcept { return next_type::crend ();              }
  
    GCH_NODISCARD riter  rend    (void)       noexcept { return m_container.rend ();              }
    GCH_NODISCARD criter rend    (void) const noexcept { return m_container.crend ();             }
    GCH_NODISCARD criter crend   (void) const noexcept { return m_container.crend ();             }
  
    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   front   (void)       noexcept { return *begin ();    }
    GCH_NODISCARD constexpr           cref&  front   (void) const noexcept { return *begin ();    }
  
    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   back    (void)       noexcept { return *(--end ());  }
    GCH_NODISCARD constexpr           cref&  back    (void) const noexcept { return *(--cend ()); }
  
    GCH_NODISCARD GCH_CPP17_CONSTEXPR std::size_t size (void) const noexcept
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
      other.set_first (new_first2);
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
      other.set_first (other.end ());
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
      other.set_first (other.end ());
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
    static void set_first (iter) noexcept { }
    static void propagate_first (citer, iter) noexcept { }
  
    citer resize_pos (size_type& count) const
    {
      citer cit;
      const size_type len = size ();
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
          difference_type num_erase = len - count;
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
    using container_type          = Container;
  
    using iterator                = typename container_type::iterator;
    using const_iterator          = typename container_type::const_iterator;
    using reverse_iterator        = typename container_type::reverse_iterator;
    using const_reverse_iterator  = typename container_type::const_reverse_iterator;
    using reference               = typename container_type::reference;
    using const_reference         = typename container_type::const_reference;
    using size_type               = typename container_type::size_type;
    using difference_type         = typename container_type::difference_type;
    using value_type              = typename container_type::value_type;
    using allocator_type          = typename container_type::allocator_type;
  
    using iter   = iterator;
    using citer  = const_iterator;
    using riter  = reverse_iterator;
    using criter = const_reverse_iterator;
    using ref    = reference;
    using cref   = const_reference;
  
    using partition_type = list_partition<value_type, N, container_type>;
    using subrange_type  = list_partition_subrange<container_type, N, Index>;
    using prev_type      = list_partition_subrange<container_type, N, Index - 1>;
    using next_type      = list_partition_subrange<container_type, N, Index + 1>;
  
    static constexpr std::size_t index (void) noexcept { return Index; }
    
  protected:
    using next_type::m_container;
  
  public:
    using next_type::num_subranges;
    
//  list_partition_subrange            (void)                               = impl;
//  list_partition_subrange            (const list_partition_subrange&)     = impl;
//  list_partition_subrange            (list_partition_subrange&&) noexcept = impl;
//  list_partition_subrange& operator= (const list_partition_subrange&)     = impl;
//  list_partition_subrange& operator= (list_partition_subrange&&) noexcept = impl;
    ~list_partition_subrange           (void)                               = default;
    
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

  protected:
    template <std::size_t M, std::size_t J, typename ...Subranges,
      typename std::enable_if<(J < M)>::type * = nullptr>
    list_partition_subrange (const list_partition_subrange<Container, M, J>& other,
                             Subranges&&... subranges)
      : next_type (next_subrange (other), std::forward<Subranges> (subranges)...),
        m_first (std::next (m_container.begin (),
                            std::distance (other.m_container.begin (), other.begin ())))
    { }
  
    template <std::size_t M, std::size_t J, typename ...Subranges,
      typename std::enable_if<(J == M)>::type * = nullptr>
    list_partition_subrange (const list_partition_subrange<Container, M, J>& other,
                             Subranges&&... subranges)
      : list_partition_subrange (std::forward<Subranges> (subranges)...)
    {
      m_container.insert (m_container.cbegin (), other.m_container.cbegin (),
                          other.m_container.cend ());
    }
  
    template <std::size_t M, std::size_t J, typename ...Subranges,
              typename std::enable_if<(J < M)>::type * = nullptr>
    list_partition_subrange (list_partition_subrange<Container, M, J>&& other,
                             Subranges&&... subranges)
      : next_type (next_subrange (other), std::forward<Subranges> (subranges)...),
        m_first (other.begin ())
    { }
  
    template <std::size_t M, std::size_t J, typename ...Subranges,
              typename std::enable_if<(J == M)>::type * = nullptr>
    list_partition_subrange (list_partition_subrange<Container, M, J>&& other,
                             Subranges&&... subranges)
      : list_partition_subrange (std::forward<Subranges> (subranges)...)
    {
      m_container.splice (m_container.cbegin (), other.m_container);
    }
  
    template <typename T, std::size_t M, typename ...Subranges>
    constexpr list_partition_subrange (list_partition<T, M, Container>&& partition,
                                       Subranges&&... subranges)
      : list_partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }
  
    template <typename T, std::size_t M, typename ...Subranges>
    constexpr list_partition_subrange (const list_partition<T, M, Container>& partition,
                                       Subranges&&... subranges)
      : list_partition_subrange (get_subrange<0> (partition),
                                 std::forward<Subranges> (subranges)...)
    { }
  
    constexpr explicit list_partition_subrange (const allocator_type& alloc)
      : next_type (alloc)
    { }
    
  public:
    list_partition_subrange& operator= (const list_partition_subrange& other) noexcept
    {
      if (&other != this)
        assign (other.begin (), other.end ());
      return *this;
    }
  
    list_partition_subrange& operator= (list_partition_subrange&& other) noexcept
    {
      if (! other.empty ())
      {
        iter last = other.begin ();
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
  
    GCH_NODISCARD riter  rbegin  (void)       noexcept { return next_type::rend ();               }
    GCH_NODISCARD criter rbegin  (void) const noexcept { return next_type::crend ();              }
    GCH_NODISCARD criter crbegin (void) const noexcept { return next_type::crend ();              }
  
    GCH_NODISCARD riter  rend    (void)       noexcept { return riter (begin ());                 }
    GCH_NODISCARD criter rend    (void) const noexcept { return criter (cbegin ());               }
    GCH_NODISCARD criter crend   (void) const noexcept { return criter (cbegin ());               }
  
    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   front   (void)       noexcept { return *begin ();    }
    GCH_NODISCARD constexpr           cref&  front   (void) const noexcept { return *begin ();    }
  
    GCH_NODISCARD GCH_CPP14_CONSTEXPR ref&   back    (void)       noexcept { return *(--end ());  }
    GCH_NODISCARD constexpr           cref&  back    (void) const noexcept { return *(--cend ()); }
  
    GCH_NODISCARD GCH_CPP17_CONSTEXPR std::size_t size (void) const noexcept
    {
      return std::distance (cbegin (), cend ());
    }
  
    GCH_NODISCARD bool empty (void) const noexcept
    {
      return cbegin () == cend ();
    }
    
    void clear (void) noexcept
    {
      set_first (m_container.erase (cbegin (), cend ()));
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
      set_first (m_container.insert (cbegin (), std::forward<T> (val)));
    }
  
    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      set_first (m_container.emplace (cbegin (), std::forward<Args> (args)...));
      return *begin ();
    }
  
    void pop_front (void)
    {
      set_first (m_container.erase (cbegin ()));
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
      set_first (new_first1);
      
      other.m_container.splice (cend (), tmp2);
      other.set_first (new_first2);
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
      set_first (new_first);
      other.set_first (other.end ());
    }
  
    template <std::size_t M, std::size_t Idx, typename ...Args>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>& other, Args&&... args)
    {
      splice (pos, std::move (other), std::forward<Args> (args)...);
    }
  
    template <std::size_t M, std::size_t Idx>
    void splice (citer pos, list_partition_subrange<container_type, M, Idx>&& other)
    {
      iter first = other.begin ();
      m_container.splice (pos, std::move (other.m_container), first, other.cend ());
      propagate_first (pos, first);
      other.set_first (other.end ());
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
      set_first (new_first);
      
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
      set_first (new_first);
      
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
      set_first (new_first);
    }
  
    template <typename ...Args>
    void sort (Args&&... args)
    {
      container_type tmp;
      tmp.splice (tmp.end (), std::move (m_container), cbegin (), cend ());
      tmp.sort (std::forward<Args> (args)...);
      iter new_first = tmp.begin ();
      m_container.splice (cend (), tmp);
      set_first (new_first);
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
    void set_first (iter replace)
    {
      prev_subrange (*this).propagate_first (m_first, replace);
      m_first = replace;
    }
    
    void propagate_first (citer cmp, iter replace)
    {
      if (m_first == cmp)
      {
        prev_subrange (*this).propagate_first (cmp, replace);
        m_first = replace;
      }
    }
    
    citer resize_pos (size_type& count) const
    {
      citer cit;
      const size_type len = size ();
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
          difference_type num_erase = len - count;
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
    using container_type          = Container;
  
    using iterator                = typename container_type::iterator;
    using const_iterator          = typename container_type::const_iterator;
    using reverse_iterator        = typename container_type::reverse_iterator;
    using const_reverse_iterator  = typename container_type::const_reverse_iterator;
    using reference               = typename container_type::reference;
    using const_reference         = typename container_type::const_reference;
    using size_type               = typename container_type::size_type;
    using difference_type         = typename container_type::difference_type;
    using value_type              = typename container_type::value_type;
    using allocator_type          = typename container_type::allocator_type;
  
    using iter   = iterator;
    using citer  = const_iterator;
    using riter  = reverse_iterator;
    using criter = const_reverse_iterator;
    using ref    = reference;
    using cref   = const_reference;
  
    using partition_type = list_partition<value_type, N, container_type>;
    using subrange_type  = list_partition_subrange<container_type, N, N>;
    using prev_type      = list_partition_subrange<container_type, N, N - 1>;
    using next_type      = void;
    
    static constexpr std::size_t index         (void) noexcept { return N; }
    static constexpr std::size_t num_subranges (void) noexcept { return N; }
    
    list_partition_subrange            (void)                               = default;
//  list_partition_subrange            (const list_partition_subrange&)     = impl;
//  list_partition_subrange            (list_partition_subrange&&) noexcept = impl;
    list_partition_subrange& operator= (const list_partition_subrange&)     = default;
    list_partition_subrange& operator= (list_partition_subrange&&) noexcept = default;
    ~list_partition_subrange           (void)                               = default;
  
  
    list_partition_subrange (const list_partition_subrange& other)
      : m_container (other.m_container)
    { }
  
    list_partition_subrange (list_partition_subrange&& other)
      : m_container (std::move (other.m_container))
    { }

  protected:
    template <std::size_t M>
    list_partition_subrange (const list_partition_subrange<container_type, M, M>& other)
      : m_container (other.m_container)
    { }
    
    template <std::size_t M>
    list_partition_subrange (list_partition_subrange<container_type, M, M>&& other)
      : m_container (std::move (other.m_container))
    { }
  
    explicit list_partition_subrange (const allocator_type& alloc)
      : m_container (alloc)
    { }
    
  public:
    GCH_NODISCARD iter   begin  (void)       noexcept { return m_container.end ();  }
    GCH_NODISCARD citer  begin  (void) const noexcept { return m_container.cend (); }
    GCH_NODISCARD citer  cbegin (void) const noexcept { return m_container.cend (); }
  
    iter   end     (void)       = delete;
    iter   end     (void) const = delete;
    citer  cend    (void)       = delete;
  
    GCH_NODISCARD riter  rend   (void)       noexcept { return m_container.rbegin ();  }
    GCH_NODISCARD criter rend   (void) const noexcept { return m_container.crbegin (); }
    GCH_NODISCARD criter crend  (void) const noexcept { return m_container.crbegin (); }
    
    iter   rbegin  (void)       = delete;
    iter   rbegin  (void) const = delete;
    citer  crbegin (void)       = delete;
  
    ref&   front   (void)       = delete;
    cref&  front   (void) const = delete;
  
    ref&   back    (void)       = delete;
    cref&  back    (void) const = delete;
  
    allocator_type get_allocator (void) const noexcept
    {
      return m_container.get_allocator ();
    }
  
    GCH_NODISCARD size_type max_size (void) const noexcept
    {
      return m_container.max_size ();
    }
  
    subrange_view<iter>  view (void)        = delete;
    subrange_view<citer> view (void) const = delete;
  protected:
    container_type m_container;
  };
  
  template <typename Container, std::size_t N, std::size_t Idx>
  void swap (list_partition_subrange<Container, N, Idx>& lhs,
             list_partition_subrange<Container, N, Idx>& rhs)
  noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }
  
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
    using cref = typename list_partition_subrange<C, N, Idx>::cref;
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
  public:
    using value_type      = T;
    using container_type  = Container;
  
    using data_iter       = typename container_type::iterator;
    using data_citer      = typename container_type::const_iterator;
    using data_riter      = typename container_type::reverse_iterator;
    using data_criter     = typename container_type::const_reverse_iterator;
    using data_ref        = typename container_type::reference;
    using data_cref       = typename container_type::const_reference;
    using data_size_type  = typename container_type::size_type;
    using data_diff_type  = typename container_type::difference_type;
    using data_value_type = typename container_type::value_type;
    using data_alloc_type = typename container_type::allocator_type;
  
    using first_type = list_partition_subrange<Container, N, 0>;
    using last_type  = list_partition_subrange<Container, N, N>;
  
    template <std::size_t Index>
    using subrange_type = list_partition_subrange<Container, N, Index>;
    
    static constexpr std::size_t num_subranges (void) noexcept { return N; }
    
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
    constexpr list_partition (const list_partition<T, M, Container>& p, Partitions&&... ps)
      : first_type (get_subrange<0> (p), std::forward<Partitions> (ps)...)
    { }

    template <std::size_t M, typename ...Partitions>
    constexpr list_partition (list_partition<T, M, Container>&& p, Partitions&&... ps)
      : first_type (get_subrange<0> (p), std::forward<Partitions> (ps)...)
    { }
  
    constexpr explicit list_partition (const data_alloc_type& alloc)
      : first_type (alloc)
    { }
  
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
  
    template <typename Subrange>
    friend constexpr parent_partition_t<Subrange>&
    get_partition (Subrange& p) noexcept;
  
    template <typename Subrange>
    friend constexpr const parent_partition_t<Subrange>&
    get_partition (const Subrange& p) noexcept;
  
    template <typename Subrange>
    friend constexpr parent_partition_t<Subrange>&&
    get_partition (Subrange&& p) noexcept;
  
    template <typename Subrange>
    friend constexpr const parent_partition_t<Subrange>&&
    get_partition (const Subrange&& p) noexcept;
  
    GCH_CPP14_CONSTEXPR subrange_type<0>& front (void) noexcept
    {
      return get_subrange<0> (*this);
    }
    
    constexpr const subrange_type<0>& front (void) const noexcept
    {
      return get_subrange<0> (*this);
    }
  
    GCH_CPP14_CONSTEXPR subrange_type<N - 1>& back (void) noexcept
    {
      return get_subrange<N - 1> (*this);
    }
    
    constexpr const subrange_type<N - 1>& back (void) const noexcept
    {
      return get_subrange<N - 1> (*this);
    }
    
    using first_type::num_subranges;
    
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
    
    data_size_type GCH_CPP17_CONSTEXPR data_size (void) const noexcept { return m_container.size (); }
    GCH_NODISCARD bool data_empty (void) const noexcept { return m_container.empty (); }
  
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
  
    static constexpr std::size_t size (void) noexcept { return N; }
    
    template <typename ...Fs>
    struct overloader
      : partition_overloader<list_partition, Fs...>
    {
      using partition_overloader<list_partition, Fs...>::partition_overloader;
      using partition_overloader<list_partition, Fs...>::operator();
    };
    
    template <typename ...Fs>
    static constexpr auto overload (Fs&&... fs) noexcept
    {
      return overloader<Fs...> (std::forward<Fs> (fs)...);
    }
    
#ifdef GCH_COMPILER_CLANG
    template <typename... Fs> overloader (Fs&&...) -> overloader<Fs...>;
#endif
    
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
