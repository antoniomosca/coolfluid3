// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Common_MPI_all_reduce_hpp
#define CF_Common_MPI_all_reduce_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Common/Foreach.hpp"
#include "Common/Assertions.hpp"
#include "Common/BasicExceptions.hpp"

#include <Common/MPI/types.hpp>
#include <Common/MPI/operations.hpp>
#include <Common/MPI/datatype.hpp>

// #include "Common/MPI/debug.hpp" // for debugging mpi

////////////////////////////////////////////////////////////////////////////////

/**
  @file all_reduce.hpp
  @author Tamas Banyai
  Doing an operation on identical sizes of data on each processes.
  For example summing up values over all the processes.
  Due to the nature of MPI standard, at the lowest level the memory required to be linear meaning &xyz[0] should give a single and continous block of memory.
  Some functions support automatic evaluation of number of items on the receive side but be very cautious with using them because it requires two collective communication and may end up with degraded performance.
  Currently, the interface supports raw pointers and std::vectors.
**/

////////////////////////////////////////////////////////////////////////////////

namespace CF {
  namespace Common {
    namespace Comm {

////////////////////////////////////////////////////////////////////////////////

namespace detail {

////////////////////////////////////////////////////////////////////////////////

  /**
    Implementation to the all reduce interface.
    Don't call this function directly, use MPI::alltoallvm instead.
    In_values and out_values must be linear in memory and their sizes should be sum(in_n[i]) and sum(out_n[i]) i=0..#processes-1.
    @param comm MPI::communicator
    @param in_values pointer to the send buffer
    @param in_n array holding send counts of size #processes
    @param in_map array of size #processes holding the mapping. If zero pointer passed, no mapping on send side.
    @param out_values pointer to the receive buffer
    @param out_n array holding receive counts of size #processes. If zero pointer passed, no mapping on receive side.
    @param out_map array of size #processes holding the mapping
    @param stride is the number of items of type T forming one array element, for example if communicating coordinates together, then stride==3:  X0,Y0,Z0,X1,Y1,Z1,...,Xn-1,Yn-1,Zn-1
  **/

  template<typename T, typename Op>
  inline void
  all_reduce_impl(const Communicator& comm,  Op, const T* in_values, const int in_n, const int *in_map, T* out_values, const int *out_map, const int stride)
  { 
    // get data type, op and some checkings
    Datatype type = get_mpi_datatype(*in_values);
    Operation op_= get_mpi_op<T,Op>::op();
    cf_assert( stride>0 );

    // there is in_map
    T *in_buf=(T*)in_values;
    if (in_map!=0){
      if ( (in_buf=new T[stride*in_n+1]) == (T*)0 ) throw CF::Common::NotEnoughMemory(FromHere(),"Could not allocate temporary buffer."); // +1 for avoiding possible zero allocation
      if (stride==1) { for(int i=0; i<in_n; i++) in_buf[i]=in_values[in_map[i]]; }
      else { for(int i=0; i<in_n; i++) memcpy(&in_buf[stride*i],&in_values[stride*in_map[i]],stride*sizeof(T)); }
    }

    // set up out_buf
    T *out_buf=out_values;
    if ((out_map!=0)||(in_values==out_values)) {
      if ( (out_buf=new T[in_n*stride+1]) == (T*)0 ) throw CF::Common::NotEnoughMemory(FromHere(),"Could not allocate temporary buffer."); // +1 for avoiding possible zero allocation
    }

    // do the communication
    MPI_CHECK_RESULT(MPI_Allreduce, ( in_buf, out_buf, in_n*stride, type, op_, comm ));

    // re-populate out_values
    if (out_map!=0) {
      if (stride==1) { for(int i=0; i<in_n; i++) out_values[out_map[i]]=out_buf[i]; }
      else { for(int i=0; i<in_n; i++) memcpy(&out_values[stride*out_map[i]],&out_buf[stride*i],stride*sizeof(T)); }
      delete[] out_buf;
    } else if (in_values==out_values) {
      memcpy(out_values,out_buf,in_n*stride*sizeof(T));
      delete[] out_buf;
    }

    // free internal memory
    if (in_map!=0) delete[] in_buf;
  }

////////////////////////////////////////////////////////////////////////////////

} // end namespace detail

////////////////////////////////////////////////////////////////////////////////

/**
  Interface to all_reduce communication with specialization to raw pointer.
  If null pointer passed for out_values then memory is allocated and the pointer to it is returned, otherwise out_values is returned.
  @param comm MPI::communicator
  @param in_values pointer to the send buffer
  @param in_n size of the send array (number of items)
  @param out_values pointer to the receive buffer
  @param stride is the number of items of type T forming one array element, for example if communicating coordinates together, then stride==3:  X0,Y0,Z0,X1,Y1,Z1,...,Xn-1,Yn-1,Zn-1
**/
template<typename T, typename Op>
inline T*
all_reduce(const Communicator& comm, const Op& op, const T* in_values, const int in_n, T* out_values, const int stride=1)
{
  // allocate out_buf if incoming pointer is null
  T* out_buf=out_values;
  if (out_values==0) {
    const int size=stride*in_n>1?stride*in_n:1;
    if ( (out_buf=new T[size]) == (T*)0 ) throw CF::Common::NotEnoughMemory(FromHere(),"Could not allocate temporary buffer.");
  }

  // call impl
  detail::all_reduce_impl(comm,op,in_values,in_n,0,out_buf,0,stride);
  return out_buf;
}

////////////////////////////////////////////////////////////////////////////////

/**
  Interface to all_reduce communication with specialization to std::vector.
  @param comm MPI::communicator
  @param in_values send buffer
  @param out_values receive buffer
  @param stride is the number of items of type T forming one array element, for example if communicating coordinates together, then stride==3:  X0,Y0,Z0,X1,Y1,Z1,...,Xn-1,Yn-1,Zn-1
**/
template<typename T, typename Op>
inline void
all_reduce(const Communicator& comm, const Op& op, const std::vector<T>& in_values, std::vector<T>& out_values, const int stride=1)
{
  // set out_values's sizes
  cf_assert( in_values.size() % stride == 0 );
  out_values.resize(in_values.size());
  out_values.reserve(in_values.size());

  // call impl
  detail::all_reduce_impl(comm, op, (T*)(&in_values[0]), in_values.size()/stride, 0, (T*)(&out_values[0]), 0, stride);
}

////////////////////////////////////////////////////////////////////////////////

/**
  Interface to the mapped all_reduce communication with specialization to raw pointer.
  If null pointer passed for out_values then memory is allocated to fit the max in map and the pointer is returned, otherwise out_values is returned.
  If out_n (receive counts) contains only -1, then a pre communication occurs to fill out_n.
  However due to the fact that map already needs all the information if you use all_to_all to allocate out_values and fill out_n then you most probably doing something wrong.
  @param comm MPI::communicator
  @param in_values pointer to the send buffer
  @param in_n array holding send counts of size #processes
  @param in_map array of size #processes holding the mapping. If zero pointer passed, no mapping on send side.
  @param out_values pointer to the receive buffer
  @param out_n array holding receive counts of size #processes. If zero pointer passed, no mapping on receive side.
  @param out_map array of size #processes holding the mapping
  @param stride is the number of items of type T forming one array element, for example if communicating coordinates together, then stride==3:  X0,Y0,Z0,X1,Y1,Z1,...,Xn-1,Yn-1,Zn-1
**/
template<typename T, typename Op>
inline T*
all_reduce(const Communicator& comm, const Op& op, const T* in_values, const int in_n, const int *in_map, T* out_values, const int *out_map, const int stride=1)
{
  // allocate out_buf if incoming pointer is null
  T* out_buf=out_values;
  if (out_values==0) {
    int out_sum=in_n;
    if (out_map!=0){
      int out_sum_tmp=0;
      for (int i=0; i<out_sum; i++) out_sum_tmp=out_map[i]>out_sum_tmp?out_map[i]:out_sum_tmp;
      out_sum=out_sum_tmp+1;
    }
    if ( (out_buf=new T[stride*out_sum]) == (T*)0 ) throw CF::Common::NotEnoughMemory(FromHere(),"Could not allocate temporary buffer.");
  }

  // call impl
  detail::all_reduce_impl(comm,op,in_values,in_n,in_map,out_buf,out_map,stride);
  return out_buf;
}

////////////////////////////////////////////////////////////////////////////////

/**
  Interface to the all_reduce communication with specialization to std::vector.
  If out_values's size is zero then its resized.
  If out_n (receive counts) is not of size of #processes, then error occurs.
  If out_n (receive counts) is filled with -1s, then a pre communication occurs to fill out_n.
  However due to the fact that map already needs all the information if you use all_to_all to allocate out_values and fill out_n then you most probably doing something wrong.
  @param comm MPI::communicator
  @param in_values send buffer
  @param in_n send counts of size #processes
  @param in_map array of size #processes holding the mapping. If zero pointer or zero size vector passed, no mapping on send side.
  @param out_values receive buffer
  @param out_n receive counts of size #processes
  @param out_map array of size #processes holding the mapping. If zero pointer or zero size vector passed, no mapping on receive side.
  @param stride is the number of items of type T forming one array element, for example if communicating coordinates together, then stride==3:  X0,Y0,Z0,X1,Y1,Z1,...,Xn-1,Yn-1,Zn-1
**/
template<typename T, typename Op>
inline void
all_reduce(const Communicator& comm, const Op& op, const std::vector<T>& in_values, const std::vector<int>& in_map, std::vector<T>& out_values, const std::vector<int>& out_map, const int stride=1)
{
  // set out_values's sizes
  cf_assert( in_values.size() % stride == 0 );

  // resize out_values if vector size is zero
  if (out_values.size() == 0 ){
    int out_sum=in_map.size();
    if (out_map.size()!=0) {
      boost_foreach( int i, out_map ) out_sum=i>out_sum?i:out_sum;
    }
    out_values.resize(stride*out_sum);
    out_values.reserve(stride*out_sum);
  }

  // call impl
  detail::all_reduce_impl(comm, op, (T*)(&in_values[0]), in_map.size(), &in_map[0], (T*)(&out_values[0]), &out_map[0], stride);
}

////////////////////////////////////////////////////////////////////////////////

    } // end namespace Comm
  } // end namespace Common
} // end namespace CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Common_MPI_all_reduce_hpp
