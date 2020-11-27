/** integer_sequence.hpp
 * Short description here. 
 * 
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GCH_INTEGER_SEQUENCE_HPP
#define GCH_INTEGER_SEQUENCE_HPP

#include <cstddef>

// based on https://stackoverflow.com/a/32223343 by Tony D
namespace gch
{
  template <std::size_t... Indices>
  struct index_sequence
  {
    using type = index_sequence;
    using value_type = std::size_t;
    static constexpr std::size_t size (void) noexcept { return sizeof...(Indices); }
  };
  
  // --------------------------------------------------------------
  
  template <class Sequence1, class Sequence2>
  struct _merge_and_renumber;
  
  template <std::size_t ...Idx1, std::size_t ...Idx2>
  struct _merge_and_renumber<index_sequence<Idx1...>, index_sequence<Idx2...>>
    : index_sequence<Idx1..., (sizeof... (Idx1) + Idx2)...>
  { };
  
  // --------------------------------------------------------------
  
  template <std::size_t N>
  struct make_index_sequence
    : _merge_and_renumber<typename make_index_sequence<(N / 2)>::type,
                          typename make_index_sequence<(N - (N / 2))>::type>
  { };
  
  template<> struct make_index_sequence<0> : index_sequence<>  { };
  template<> struct make_index_sequence<1> : index_sequence<0> { };
}

#endif // PARTITION_INTEGER_SEQUENCE_HPP
