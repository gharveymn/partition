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

namespace gch
{
  template <typename Container, std::size_t N, std::size_t Index, typename Enable = void>
  class vector_partition_subrange;
  
  template <typename T, std::size_t N, typename Container = std::vector<T>>
  class vector_partition;
  
  template <typename Container, std::size_t N>
  class vector_partition_subrange<Container, N, 0>
    : public vector_partition_subrange<Container, N, 1>
  {
    template <typename, std::size_t, std::size_t, typename>
    friend class vector_partition_subrange;
  
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
    
    using partition_type = vector_partition<value_type, N, container_type>;
    using subrange_type  = vector_partition_subrange<container_type, N, 0>;
    using prev_type      = void;
    using next_type      = vector_partition_subrange<container_type, N, 1>;
    
    static constexpr std::size_t index (void) noexcept { return 0; }
  
  protected:
    using next_type::m_container;
  
  public:
    using next_type::num_subranges;

//  vector_partition_subrange            (void)                               = impl;
//  vector_partition_subrange            (const vector_partition_subrange&)     = impl;
//  vector_partition_subrange            (vector_partition_subrange&&) noexcept = impl;
//  vector_partition_subrange& operator= (const vector_partition_subrange&)     = impl;
//  vector_partition_subrange& operator= (vector_partition_subrange&&) noexcept = impl;
    ~vector_partition_subrange           (void)                               = default;
    
    constexpr vector_partition_subrange (void)
      : next_type ()
    { }
    
    constexpr vector_partition_subrange (const vector_partition_subrange& other)
      : next_type (other)
    { }
    
    constexpr vector_partition_subrange (vector_partition_subrange&& other)
      : next_type (std::move (other))
    { }

  protected:
    
    
    template <std::size_t M, typename ...Subranges>
    constexpr vector_partition_subrange (const vector_partition_subrange<Container, M, 0>& other,
                                       Subranges&&... subranges)
      : next_type (0, next_subrange (other), std::forward<Subranges> (subranges)...)
    { }
    
    template <std::size_t M, typename ...Subranges>
    constexpr vector_partition_subrange (vector_partition_subrange<Container, M, 0>&& other,
                                       Subranges&&... subranges)
      : next_type (0, next_subrange (other), std::forward<Subranges> (subranges)...)
    { }
    
    template <typename T, std::size_t M, typename ...Subranges>
    constexpr vector_partition_subrange (vector_partition<T, M, Container>&& partition,
                                       Subranges&&... subranges)
      : vector_partition_subrange (get_subrange<0> (partition),
                                   std::forward<Subranges> (subranges)...)
    { }
    
    template <typename T, std::size_t M, typename ...Subranges>
    constexpr vector_partition_subrange (const vector_partition<T, M, Container>& partition,
                                       Subranges&&... subranges)
      : vector_partition_subrange (get_subrange<0> (partition),
                                   std::forward<Subranges> (subranges)...)
    { }
    
    constexpr explicit vector_partition_subrange (const allocator_type& alloc)
      : next_type (alloc)
    { }
  
  public:
    vector_partition_subrange& operator= (const vector_partition_subrange& other) noexcept
    {
      if (&other != this)
        assign (other.begin (), other.end ());
      return *this;
    }
    
    vector_partition_subrange& operator= (vector_partition_subrange&& other) noexcept
    {
      assign (std::make_move_iterator (other.begin ()), std::make_move_iterator (other.end ()));
      return *this;
    }
    
    explicit vector_partition_subrange (iter) { }
    
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
      size_type sz = size ();
      m_container.erase (cbegin (), cend ());
      next_subrange (*this).add_to_offset (-sz);
    }
    
    template <typename ...Args>
    iter insert (const citer pos, Args&&... args)
    {
      size_type sz = m_container.size ();
      iter ret = m_container.insert (pos, std::forward<Args> (args)...);
      next_subrange (*this).add_to_offset (m_container.size () - sz);
      return ret;
    }
    
    template <typename ...Args>
    iter emplace (const citer pos, Args&&... args)
    {
      next_subrange (*this).add_to_offset (1);
      return m_container.emplace (pos, std::forward<Args> (args)...);
    }
    
    iter erase (const citer pos)
    {
      next_subrange (*this).add_to_offset (-1);
      return m_container.erase (pos);
    }
    
    iter erase (const citer first, const citer last)
    {
      next_subrange (*this).add_to_offset (std::distance (first, last));
      return m_container.erase (first, last);
    }
    
    template <typename T>
    void push_back (T&& val)
    {
      m_container.insert (end (), std::forward<T> (val));
      next_subrange (*this).add_to_offset (1);
    }
    
    template <typename ...Args>
    ref emplace_back (Args&&... args)
    {
      iter ret = m_container.emplace (end (), std::forward<Args> (args)...);
      next_subrange (*this).add_to_offset (1);
      return *ret;
    }
    
    void pop_back (void)
    {
      erase (--cend ());
    }
    
    template <typename T>
    void push_front (T&& val)
    {
      m_container.push_front (std::forward<T> (val));
      next_subrange (*this).add_to_offset (1);
    }
    
    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      iter ret = m_container.emplace (begin (), std::forward<Args> (args)...);
      next_subrange (*this).add_to_offset (1);
      return *ret;
    }
    
    template <std::size_t M, std::size_t Idx>
    void swap (vector_partition_subrange<container_type, M, Idx>& other)
    noexcept (noexcept (std::declval<container_type> ().swap (std::declval<container_type&> ())))
    {
      if (&other == this)
        return;
  
      container_type tmp (std::make_move_iterator (begin ()), std::make_move_iterator (end ()));
      assign (std::make_move_iterator (other.begin ()), std::make_move_iterator (other.end ()));
      other.assign (std::make_move_iterator (tmp.begin ()), std::make_move_iterator (tmp.end ()));
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
    
  protected:
    constexpr size_type get_offset (void) const noexcept { return 0; }
    
  private:
    void propagate_offset (size_type, difference_type) { }
    
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
  class vector_partition_subrange<Container, N, Index,
                                typename std::enable_if<(0 < Index) && (Index < N)>::type>
    : public vector_partition_subrange<Container, N, Index + 1>
  {
    template <typename, std::size_t, std::size_t, typename>
    friend class vector_partition_subrange;
  
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
    
    using partition_type = vector_partition<value_type, N, container_type>;
    using subrange_type  = vector_partition_subrange<container_type, N, Index>;
    using prev_type      = vector_partition_subrange<container_type, N, Index - 1>;
    using next_type      = vector_partition_subrange<container_type, N, Index + 1>;
    
    static constexpr std::size_t index (void) noexcept { return Index; }
  
  protected:
    using next_type::m_container;
  
  public:
    using next_type::num_subranges;

    vector_partition_subrange            (void)                                 = default;
    vector_partition_subrange            (const vector_partition_subrange&)     = default;
    vector_partition_subrange            (vector_partition_subrange&&) noexcept = default;
//  vector_partition_subrange& operator= (const vector_partition_subrange&)     = impl;
//  vector_partition_subrange& operator= (vector_partition_subrange&&) noexcept = impl;
    ~vector_partition_subrange           (void)                                 = default;

  protected:
    template <std::size_t M, std::size_t J, typename ...Subranges,
      typename std::enable_if<(J < M)>::type * = nullptr>
    vector_partition_subrange (size_type accum,
                               const vector_partition_subrange<Container, M, J>& other,
                               Subranges&&... subranges)
      : next_type (accum, next_subrange (other), std::forward<Subranges> (subranges)...),
        m_offset (other.m_offset + accum)
    { }
    
    template <std::size_t M, std::size_t J, typename ...Subranges,
      typename std::enable_if<(J == M)>::type * = nullptr>
    vector_partition_subrange (size_type accum,
                               const vector_partition_subrange<Container, M, J>& other,
                               Subranges&&... subranges)
      : vector_partition_subrange (accum + other.m_container.size (),
                                   std::forward<Subranges> (subranges)...)
    { }
    
    template <std::size_t M, std::size_t J, typename ...Subranges,
      typename std::enable_if<(J < M)>::type * = nullptr>
    vector_partition_subrange (size_type accum,
                               vector_partition_subrange<Container, M, J>&& other,
                               Subranges&&... subranges)
      : next_type (accum, next_subrange (other), std::forward<Subranges> (subranges)...),
        m_offset (other.m_offset + accum)
    { }
    
    template <std::size_t M, std::size_t J, typename ...Subranges,
      typename std::enable_if<(J == M)>::type * = nullptr>
    vector_partition_subrange (size_type accum,
                               vector_partition_subrange<Container, M, J>&& other,
                               Subranges&&... subranges)
      : vector_partition_subrange (accum + other.m_container.size (),
                                   std::forward<Subranges> (subranges)...)
    { }
    
    template <typename T, std::size_t M, typename ...Subranges>
    constexpr vector_partition_subrange (size_type accum,
                                         vector_partition<T, M, Container>&& partition,
                                         Subranges&&... subranges)
      : vector_partition_subrange (accum, get_subrange<0> (partition),
                                   std::forward<Subranges> (subranges)...)
    { }
    
    template <typename T, std::size_t M, typename ...Subranges>
    constexpr vector_partition_subrange (size_type accum,
                                         const vector_partition<T, M, Container>& partition,
                                         Subranges&&... subranges)
      : vector_partition_subrange (accum, get_subrange<0> (partition),
                                   std::forward<Subranges> (subranges)...)
    { }
    
    constexpr explicit vector_partition_subrange (const allocator_type& alloc)
      : next_type (alloc)
    { }
  
  public:
    vector_partition_subrange& operator= (const vector_partition_subrange& other) noexcept
    {
      if (&other != this)
        assign (other.begin (), other.end ());
      return *this;
    }
    
    vector_partition_subrange& operator= (vector_partition_subrange&& other) noexcept
    {
      assign (std::make_move_iterator (other.begin ()), std::make_move_iterator (other.end ()));
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
      size_type sz = size ();
      m_container.erase (cbegin (), cend ());
      modify_offsets (m_offset, -sz);
    }
    
    template <typename ...Args>
    iter insert (const citer pos, Args&&... args)
    {
      size_type pos_off = compute_absolute_offset (pos);
      size_type sz = m_container.size ();
      iter ret = m_container.insert (pos, std::forward<Args> (args)...);
      modify_offsets (pos_off, m_container.size () - sz);
      return ret;
    }
    
    template <typename ...Args>
    iter emplace (const citer pos, Args&&... args)
    {
      size_type pos_off = compute_absolute_offset (pos);
      iter ret = m_container.emplace (pos, std::forward<Args> (args)...);
      modify_offsets (pos_off, 1);
      return ret;
    }
    
    iter erase (const citer pos)
    {
      size_type pos_off = compute_absolute_offset (pos);
      iter ret = m_container.erase (pos);
      modify_offsets (pos_off, -1);
      return ret;
    }
    
    iter erase (const citer first, const citer last)
    {
      size_type pos_off      = compute_absolute_offset (first);
      difference_type change = std::distance (first, last);
      iter ret = m_container.erase (first, last);
      modify_offsets (pos_off, -change);
      return ret;
    }
    
    template <typename T>
    void push_back (T&& val)
    {
      size_type pos_off = next_subrange (*this).get_offset ();
      iter it = m_container.insert (cend (), std::forward<T> (val));
      modify_offsets (pos_off, 1);
    }
    
    template <typename ...Args>
    ref emplace_back (Args&&... args)
    {
      size_type pos_off = next_subrange (*this).get_offset ();
      iter ret = m_container.emplace (cend (), std::forward<Args> (args)...);
      modify_offsets (pos_off, 1);
      return *ret;
    }
    
    void pop_back (void)
    {
      erase (--cend ());
    }
    
    template <typename T>
    void push_front (T&& val)
    {
      m_container.insert (cbegin (), std::forward<T> (val));
      modify_offsets (cbegin (), 1);
    }
    
    template <typename ...Args>
    ref emplace_front (Args&&... args)
    {
      m_container.emplace (cbegin (), std::forward<Args> (args)...);
      modify_offsets (cbegin (), 1);
      return *begin ();
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
    void swap (vector_partition_subrange<container_type, M, Idx>& other)
    noexcept (noexcept (std::declval<container_type> ().swap (std::declval<container_type&> ())))
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

  protected:
    constexpr size_type get_offset (void) const noexcept { return m_offset; }
    
  private:
    void modify_offsets (size_type pos_off, difference_type change)
    {
      if (change == 0)
        return;
      
      next_subrange (*this).add_to_offset (change);
      //propagate_offset (pos_off, change);
    }
  
    void propagate_offset (size_type pos_off, difference_type change)
    {
      if (m_offset == pos_off)
      {
        prev_subrange (*this).propagate_offset (pos_off, change);
        m_offset += change;
      }
    }
    
    void add_to_offset (difference_type change)
    {
      next_subrange (*this).add_to_offset (change);
      m_offset += change;
    }
    
    GCH_CPP17_CONSTEXPR size_type compute_absolute_offset (citer it) const noexcept
    {
      return it - m_container.cbegin ();
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
    
    size_type m_offset = 0;
  };
  
  // end case holds the actual container
  template <typename Container, std::size_t N>
  class vector_partition_subrange<Container, N, N>
  {
    template <typename, std::size_t, std::size_t, typename>
    friend class vector_partition_subrange;
  
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
    
    using partition_type = vector_partition<value_type, N, container_type>;
    using subrange_type  = vector_partition_subrange<container_type, N, N>;
    using prev_type      = vector_partition_subrange<container_type, N, N - 1>;
    using next_type      = void;
    
    static constexpr std::size_t index         (void) noexcept { return N; }
    static constexpr std::size_t num_subranges (void) noexcept { return N; }
    
    vector_partition_subrange            (void)                                 = default;
//  vector_partition_subrange            (const vector_partition_subrange&)     = impl;
//  vector_partition_subrange            (vector_partition_subrange&&) noexcept = impl;
    vector_partition_subrange& operator= (const vector_partition_subrange&)     = default;
    vector_partition_subrange& operator= (vector_partition_subrange&&) noexcept = default;
    ~vector_partition_subrange           (void)                                 = default;
    
    GCH_CPP20_CONSTEXPR
    vector_partition_subrange (const vector_partition_subrange& other) GCH_CPP17_NOEXCEPT
      : m_container (other.m_container)
    { }
  
    GCH_CPP20_CONSTEXPR
    vector_partition_subrange (vector_partition_subrange&& other) GCH_CPP17_NOEXCEPT
      : m_container (std::move (other.m_container))
    { }

  protected:
    template <std::size_t M>
    GCH_CPP20_CONSTEXPR
    vector_partition_subrange (size_type,
                               const vector_partition_subrange<container_type, M, M>& other)
      GCH_CPP17_NOEXCEPT
      : m_container { }
    { }
    
    template <std::size_t M>
    GCH_CPP20_CONSTEXPR
    vector_partition_subrange (size_type,
                               vector_partition_subrange<container_type, M, M>&& other)
      GCH_CPP17_NOEXCEPT
      : m_container { }
    { }
  
    GCH_CPP20_CONSTEXPR explicit
    vector_partition_subrange (const allocator_type& alloc) GCH_CPP17_NOEXCEPT
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
  
    GCH_CPP20_CONSTEXPR allocator_type get_allocator (void) const noexcept
    {
      return m_container.get_allocator ();
    }
  
    GCH_NODISCARD GCH_CPP20_CONSTEXPR size_type max_size (void) const noexcept
    {
      return m_container.max_size ();
    }
    
    subrange_view<iter>  view (void)       = delete;
    subrange_view<citer> view (void) const = delete;

  protected:
    constexpr size_type get_offset (void) const noexcept { return m_container.size (); }
    
  private:
    static void add_to_offset (difference_type) noexcept { }
    
  protected:
    container_type m_container;
  };
  
  template <typename Container, std::size_t N, std::size_t Idx>
  void swap (vector_partition_subrange<Container, N, Idx>& lhs,
             vector_partition_subrange<Container, N, Idx>& rhs)
  noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  void swap (vector_partition_subrange<Container, N, Idx1>& lhs,
             vector_partition_subrange<Container, M, Idx2>& rhs)
  noexcept (noexcept (lhs.swap (rhs)))
  {
    lhs.swap (rhs);
  }
  
  template <typename C, std::size_t N, std::size_t Idx, typename T>
  typename vector_partition_subrange<C, N, Idx>::size_type
  erase (vector_partition_subrange<C, N, Idx>& c, const T& val)
  {
    auto it = std::remove (c.begin (), c.end (), val);
    auto r  = std::distance (it, c.end ());
    c.erase (it, c.end ());
    return r;
  }
  
  template <typename C, std::size_t N, std::size_t Idx, typename Pred>
  typename vector_partition_subrange<C, N, Idx>::size_type
  erase (vector_partition_subrange<C, N, Idx>& c, Pred pred)
  {
    auto it = std::remove_if (c.begin(), c.end(), pred);
    auto r  = std::distance (it, c.end());
    c.erase (it, c.end ());
    return r;
  }
  
  template <typename T, std::size_t N, typename Container>
  class vector_partition
    : protected vector_partition_subrange<Container, N, 0>
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
  
    using first_type = vector_partition_subrange<Container, N, 0>;
    using last_type  = vector_partition_subrange<Container, N, N>;
  
    template <std::size_t Index>
    using subrange_type = vector_partition_subrange<Container, N, Index>;
  
    static constexpr std::size_t num_subranges (void) noexcept { return N; }
  
  protected:
    using vector_partition_subrange<Container, N, 0>::m_container;
  
  public:
    vector_partition            (void)                      = default;
    vector_partition            (const vector_partition&)     = default;
    vector_partition            (vector_partition&&) noexcept = default;
    vector_partition& operator= (const vector_partition&)     = default;
    vector_partition& operator= (vector_partition&&) noexcept = default;
    ~vector_partition           (void)                      = default;
    
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
  
    void construct (void) const noexcept { }

  public:
    constexpr explicit vector_partition (const data_alloc_type& alloc)
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
    
    data_size_type data_size (void) const noexcept { return m_container.size (); }
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
    struct overloader
      : partition_overloader<vector_partition, Fs...>
    {
      using partition_overloader<vector_partition, Fs...>::partition_overloader;
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
  constexpr auto get (vector_partition<T, N, Container>& p) noexcept
  -> decltype (get_subrange<Index> (p))
  {
    return get_subrange<Index> (p);
  }
  
  template <std::size_t Index, typename T, std::size_t N, typename Container>
  constexpr auto get (const vector_partition<T, N, Container>& p) noexcept
  -> decltype (get_subrange<Index> (p))
  {
    return get_subrange<Index> (p);
  }
  
  template <std::size_t Index, typename T, std::size_t N, typename Container>
  constexpr auto get (vector_partition<T, N, Container>&& p) noexcept
  -> decltype (get_subrange<Index> (p))
  {
    return get_subrange<Index> (p);
  }
  
  template <std::size_t Index, typename T, std::size_t N, typename Container>
  constexpr auto get (const vector_partition<T, N, Container>&& p) noexcept
  -> decltype (get_subrange<Index> (p))
  {
    return get_subrange<Index> (p);
  }
  
  template <typename T, typename Container, typename ...Partitions>
  vector_partition<T, total_subranges<Partitions...>::value, Container>
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
  constexpr bool operator== (const vector_partition_subrange<Container, N, Idx1>& lhs,
                             const vector_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator<= (const vector_partition_subrange<Container, N, Idx1>& lhs,
                    const vector_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::lexicographical_compare_three_way (lhs.begin (), lhs.end (),
                                                   rhs.begin (), rhs.end (),
                                                   detail::compare::synth_three_way);
  }

#else
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator== (const vector_partition_subrange<Container, N, Idx1>& lhs,
                   const vector_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::equal (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator!= (const vector_partition_subrange<Container, N, Idx1>& lhs,
                   const vector_partition_subrange<Container, M, Idx2>& rhs)
  {
    return ! (lhs == rhs);
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator< (const vector_partition_subrange<Container, N, Idx1>& lhs,
                  const vector_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::lexicographical_compare (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator<= (const vector_partition_subrange<Container, N, Idx1>& lhs,
                   const vector_partition_subrange<Container, M, Idx2>& rhs)
  {
    return ! (lhs > rhs);
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator> (const vector_partition_subrange<Container, N, Idx1>& lhs,
                  const vector_partition_subrange<Container, M, Idx2>& rhs)
  {
    return std::lexicographical_compare (rhs.begin (), rhs.end (), lhs.begin (), lhs.end ());
  }
  
  template <typename Container, std::size_t N, std::size_t Idx1, std::size_t M, std::size_t Idx2>
  bool operator>= (const vector_partition_subrange<Container, N, Idx1>& lhs,
                   const vector_partition_subrange<Container, M, Idx2>& rhs)
  {
    return ! (lhs < rhs);
  }

#endif
}

#endif // PARTITION_VECTOR_PARTITION_HPP
