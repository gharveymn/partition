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
  class partition_subrange_impl<std::list<ListArgs...>, N, 0>
  {
  protected:
    using Container     = std::list<ListArgs...>;
    using subrange_type = partition_subrange<Container, N, 0>;
    using next_type     = partition_subrange<Container, N, 1>;

  public:
    using iter       = typename Container::iterator;
    using citer      = typename Container::const_iterator;
    using riter      = typename Container::reverse_iterator;
    using criter     = typename Container::const_reverse_iterator;
    using ref        = typename Container::reference;
    using cref       = typename Container::const_reference;
    using size_type  = typename Container::size_type;
    using value_type = typename Container::value_type;
  
    partition_subrange_impl            (void)                               = delete;
    partition_subrange_impl            (const partition_subrange_impl&)     = default;
    partition_subrange_impl            (partition_subrange_impl&&) noexcept = default;
//  partition_subrange_impl& operator= (const partition_subrange_impl&)     = default;
//  partition_subrange_impl& operator= (partition_subrange_impl&&) noexcept = impl;
    ~partition_subrange_impl           (void)                               = default;
  
    partition_subrange_impl& operator= (const partition_subrange_impl& other) noexcept
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
  
    partition_subrange_impl& operator= (partition_subrange_impl&& other) noexcept
    {
      clear ();
      get_container (this).splice (cend (), other.get_container (this), other.begin (), other.end ());
      return *this;
    }
  
    // TODO: assign
  
    explicit partition_subrange_impl (iter)
    { }
  
    GCH_NODISCARD iter  begin  (void)       noexcept { return get_container (this).begin (); }
    GCH_NODISCARD citer begin  (void) const noexcept { return get_container (this).cbegin (); }
    GCH_NODISCARD citer cbegin (void) const noexcept { return get_container (this).cbegin (); }
  
    void clear (void) noexcept
    {
      get_container (this).erase (cbegin (), cend ());
    }
  
    template <typename ...Args>
    iter insert (const citer pos, Args&&... args)
    {
      return get_container (this).insert (pos, std::forward<Args> (args)...);
    }
  
    template <typename ...Args>
    iter emplace (const citer pos, Args&&... args)
    {
      return get_container (this).emplace (pos, std::forward<Args> (args)...);
    }
  
    iter erase (const citer pos)
    {
      return get_container (this).erase (pos);
    }
  
    iter erase (const citer first, const citer last)
    {
      return get_container (this).erase (first, last);
    }
  
    template <typename T>
    void push_back (T&& val)
    {
      get_container (this).insert (end (), std::forward<T> (val));
    }
  
    template <typename ...Args>
    ref emplace_back (Args&&... args)
    {
      return *get_container (this).emplace (end (), std::forward<Args> (args)...);
    }
  
    void pop_back (void)
    {
      erase (--cend ());
    }
  
    template <typename T>
    void push_front (T&& val)
    {
      get_container (this).push_front (std::forward<T> (val));
    }
  
    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      return *get_container (this).emplace (begin (), std::forward<Args> (args)...);
    }
  
    void pop_front (void)
    {
      get_container (this).pop_front ();
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
  
    GCH_NODISCARD iter  end  (void)       noexcept { return subrange_cast (this)->end ();  }
    GCH_NODISCARD citer cend (void) const noexcept { return subrange_cast (this)->cend (); }
  
    static constexpr Container& get_container (partition_subrange_impl *p) noexcept
    {
      return subrange_cast (p)->m_container;
    }
  
    static constexpr const Container& get_container (const partition_subrange_impl *p) noexcept
    {
      return subrange_cast (p)->m_container;
    }
  
    static constexpr subrange_type *subrange_cast (partition_subrange_impl *p) noexcept
    {
      return static_cast<subrange_type *> (p);
    }
  
    static constexpr const subrange_type *subrange_cast (const partition_subrange_impl *p) noexcept
    {
      return static_cast<const subrange_type *> (p);
    }
  };
  
  template <std::size_t N, std::size_t Index, typename ...ListArgs>
  class partition_subrange_impl<std::list<ListArgs...>, N, Index,
                                typename std::enable_if<Index != 0>::type>
  {
  protected:
    using Container     = std::list<ListArgs...>;
    using subrange_type = partition_subrange<Container, N, Index>;
    using next_type     = partition_subrange<Container, N, Index + 1>;
  
  public:
    using iter       = typename Container::iterator;
    using citer      = typename Container::const_iterator;
    using riter      = typename Container::reverse_iterator;
    using criter     = typename Container::const_reverse_iterator;
    using ref        = typename Container::reference;
    using cref       = typename Container::const_reference;
    using size_type  = typename Container::size_type;
    using value_type = typename Container::value_type;
    
    partition_subrange_impl            (void)                               = delete;
    partition_subrange_impl            (const partition_subrange_impl&)     = default;
    partition_subrange_impl            (partition_subrange_impl&&) noexcept = default;
//  partition_subrange_impl& operator= (const partition_subrange_impl&)     = default;
//  partition_subrange_impl& operator= (partition_subrange_impl&&) noexcept = impl;
    ~partition_subrange_impl           (void)                               = default;
  
    partition_subrange_impl& operator= (const partition_subrange_impl& other) noexcept
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
  
    partition_subrange_impl& operator= (partition_subrange_impl&& other) noexcept
    {
      clear ();
      get_container (this).splice (cend (), other.get_container (this), other.begin (), other.end ());
      other.m_front = other.end ();
      return *this;
    }
    
    // TODO: assign
    
    explicit partition_subrange_impl (iter it)
      : m_front (it)
    { }
  
    GCH_NODISCARD iter  begin  (void)       noexcept { return m_front; }
    GCH_NODISCARD citer begin  (void) const noexcept { return citer (m_front); }
    GCH_NODISCARD citer cbegin (void) const noexcept { return citer (m_front); }
    
    void clear (void) noexcept
    {
      m_front = get_container (this).erase (cbegin (), cend ());
    }
    
    template <typename ...Args>
    iter insert (const citer pos, Args&&... args)
    {
      auto ret = get_container (this).insert (pos, std::forward<Args> (args)...);
      if (pos == m_front)
        m_front = ret;
      return ret;
    }
    
    template <typename ...Args>
    iter emplace (const citer pos, Args&&... args)
    {
      auto ret = get_container (this).emplace (pos, std::forward<Args> (args)...);
      if (pos == m_front)
        m_front = ret;
      return ret;
    }
  
    iter erase (const citer pos)
    {
      auto ret = get_container (this).erase (pos);
      if (pos == m_front)
        m_front = ret;
      return ret;
    }
  
    iter erase (const citer first, const citer last)
    {
      auto ret = get_container (this).erase (first, last);
      if (first == m_front)
        m_front = ret;
      return ret;
    }
    
    template <typename T>
    void push_back (T&& val)
    {
      if (m_front == end ())
        m_front = get_container (this).insert (m_front, std::forward<T> (val));
      else
        get_container (this).insert (cend (), std::forward<T> (val));
    }
  
    template <typename ...Args>
    ref emplace_back (Args&&... args)
    {
      auto it = get_container (this).emplace (cend (), std::forward<Args> (args)...);
      if (m_front == end ())
        m_front = it;
      return *it;
    }
    
    void pop_back (void)
    {
      erase (--cend ());
    }
    
    template <typename T>
    void push_front (T&& val)
    {
      m_front = get_container (this).insert (m_front, std::forward<T> (val));
    }
  
    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      m_front = get_container (this).emplace (m_front, std::forward<Args> (args)...);
      return *m_front;
    }
  
    void pop_front (void)
    {
      m_front = erase (m_front);
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
  
    GCH_NODISCARD iter  end  (void)       noexcept { return subrange_cast (this)->end ();  }
    GCH_NODISCARD citer cend (void) const noexcept { return subrange_cast (this)->cend (); }
  
    static constexpr Container& get_container (partition_subrange_impl *p) noexcept
    {
      return subrange_cast (p)->m_container;
    }
  
    static constexpr const Container& get_container (const partition_subrange_impl *p) noexcept
    {
      return subrange_cast (p)->m_container;
    }
  
    static constexpr subrange_type *subrange_cast (partition_subrange_impl *p) noexcept
    {
      return static_cast<subrange_type *> (p);
    }
  
    static constexpr const subrange_type *subrange_cast (const partition_subrange_impl *p) noexcept
    {
      return static_cast<const subrange_type *> (p);
    }
    
    iter m_front;
  };
}

#endif // PARTITION_LIST_PARTITION_HPP
