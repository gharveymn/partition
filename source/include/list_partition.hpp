/** list_partition.hpp
 * Short description here. 
 * 
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef PARTITION_LIST_PARTITION_HPP
#define PARTITION_LIST_PARTITION_HPP

#include <partition.hpp>

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
  template <std::size_t N, typename ...ListArgs>
  class partition_subrange<std::list<ListArgs...>, N, 0>
    : public partition_subrange<std::list<ListArgs...>, N, 1>
  {
    friend partition_subrange<std::list<ListArgs...>, N, 1>;
  protected:
    using container_type = std::list<ListArgs...>;
    using subrange_type  = partition_subrange<container_type, N, 0>;
    using next_type      = partition_subrange<container_type, N, 1>;
    
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
  
    partition_subrange            (void)                               = default;
    partition_subrange            (const partition_subrange&)     = default;
    partition_subrange            (partition_subrange&&) noexcept = default;
//  partition_subrange& operator= (const partition_subrange&)     = default;
//  partition_subrange& operator= (partition_subrange&&) noexcept = impl;
    ~partition_subrange           (void)                               = default;
  
    partition_subrange& operator= (const partition_subrange& other) noexcept
    {
      if (&other != this)
      {
        auto it1 = begin ();
        auto it2 = other.begin ();
        for (; it1 != end () && it2 != other.end (); ++it1, ++it2)
          *it1 = *it2;
        erase (it1, end ());
        insert (it2, other.end ());
      }
      return *this;
    }
  
    partition_subrange& operator= (partition_subrange&& other) noexcept
    {
      clear ();
      m_container.splice (cend (), other.m_container, other.begin (), other.end ());
      return *this;
    }
  
    alloc_type get_allocator (void) const noexcept
    {
      return m_container.get_allocator ();
    }
  
    // TODO: assign
  
    explicit partition_subrange (iter)
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

// TODO merge, splice, remove, remove_if, reverse, unique, sort

  private:
  
    void propagate_front (iter, iter) { }
  
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
          auto num_erase = len - count;
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
  
  template <std::size_t N, std::size_t Index, typename ...ListArgs>
  class partition_subrange<std::list<ListArgs...>, N, Index,
                                typename std::enable_if<Index != 0 && Index != N>::type>
    : public partition_subrange<std::list<ListArgs...>, N, Index + 1>
  {
    friend partition_subrange<std::list<ListArgs...>, N, Index + 1>;
  protected:
    using container_type     = std::list<ListArgs...>;
    using subrange_type = partition_subrange<container_type, N, Index>;
    using prev_type     = partition_subrange<container_type, N, Index - 1>;
    using next_type     = partition_subrange<container_type, N, Index + 1>;
    
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
    
//  partition_subrange            (void)                               = impl;
    partition_subrange            (const partition_subrange&)     = default;
    partition_subrange            (partition_subrange&&) noexcept = default;
//  partition_subrange& operator= (const partition_subrange&)     = impl;
//  partition_subrange& operator= (partition_subrange&&) noexcept = impl;
    ~partition_subrange           (void)                               = default;
    
    partition_subrange (void)
      : next_type (),
        m_front (m_container.end ())
    { }
  
    partition_subrange& operator= (const partition_subrange& other) noexcept
    {
      if (&other != this)
      {
        auto it1 = begin ();
        auto it2 = other.begin ();
        for (; it1 != end () && it2 != other.end (); ++it1, ++it2)
          *it1 = *it2;
        erase (it1, end ());
        insert (it2, other.end ());
      }
      return *this;
    }
  
    partition_subrange& operator= (partition_subrange&& other) noexcept
    {
      clear ();
      m_container.splice (cend (), other.m_container, other.begin (), other.end ());
      other.m_front = other.end ();
      return *this;
    }
  
    alloc_type get_allocator (void) const noexcept
    {
      return m_container.get_allocator ();
    }
    
    // TODO: assign
  
    GCH_NODISCARD iter  begin  (void)       noexcept { return m_front;                            }
    GCH_NODISCARD citer begin  (void) const noexcept { return citer (m_front);                    }
    GCH_NODISCARD citer cbegin (void) const noexcept { return citer (m_front);                    }
  
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
      absolute_propagate_front (m_container.erase (cbegin (), cend ()));
    }
    
    template <typename ...Args>
    iter insert (const citer pos, Args&&... args)
    {
      auto ret = m_container.insert (pos, std::forward<Args> (args)...);
      propagate_front (pos, ret);
      return ret;
    }
    
    template <typename ...Args>
    iter emplace (const citer pos, Args&&... args)
    {
      auto ret = m_container.emplace (pos, std::forward<Args> (args)...);
      propagate_front (pos, ret);
      return ret;
    }
  
    iter erase (const citer pos)
    {
      auto ret = m_container.erase (pos);
      propagate_front (pos, ret);
      return ret;
    }
  
    iter erase (const citer first, const citer last)
    {
      auto ret = m_container.erase (first, last);
      propagate_front (first, ret);
      return ret;
    }
    
    template <typename T>
    void push_back (T&& val)
    {
      auto it = m_container.insert (cend (), std::forward<T> (val));
      propagate_front (cend (), it);
    }
  
    template <typename ...Args>
    ref emplace_back (Args&&... args)
    {
      auto ret = m_container.emplace (cend (), std::forward<Args> (args)...);
      propagate_front (end (), ret);
      return *ret;
    }
    
    void pop_back (void)
    {
      erase (--cend ());
    }
    
    template <typename T>
    void push_front (T&& val)
    {
      absolute_propagate_front (m_container.insert (m_front, std::forward<T> (val)));
    }
  
    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      absolute_propagate_front (m_container.emplace (m_front, std::forward<Args> (args)...));
      return *m_front;
    }
  
    void pop_front (void)
    {
      absolute_propagate_front (m_container.erase (m_front));
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

// TODO merge, splice, remove, remove_if, reverse, unique, sort
    
  private:
  
    void absolute_propagate_front (iter replace)
    {
      static_cast<prev_type *> (this)->propagate_front (m_front, replace);
      m_front = replace;
    }
    
    void propagate_front (iter cmp, iter replace)
    {
      if (m_front == cmp)
      {
        static_cast<prev_type *> (this)->propagate_front (cmp, replace);
        m_front = replace;
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
          auto num_erase = len - count;
          std::advance (cit, -num_erase);
        }
        count = 0;
        return cit;
      }
      cit = cend ();
      count -= len;
      return cit;
    }
    
    iter m_front;
  };
}

#endif // PARTITION_LIST_PARTITION_HPP
