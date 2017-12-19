/*
 *  Created by avlechen (c), 2017
 *  MIT license, so do whatever you want.
 * 
 *  This code implements sorting of types if some precedence order is defined
 */

// TODO use static-list.hpp for all service functions

#include <iostream>
#include <typeinfo>

#include "static-list.hpp"

template<bool c, typename NewT, typename TrueClsT, typename FalseClsT>
struct Classify{
};
template<typename NewT, typename TrueClsT, typename FalseClsT>
struct Classify<true, NewT, TrueClsT, FalseClsT>{
  typedef typename PushFront<NewT, TrueClsT>::Result NewTrueClsT;
  typedef FalseClsT  NewFalseClsT;
};
template<typename NewT, typename TrueClsT, typename FalseClsT>
struct Classify<false, NewT, TrueClsT, FalseClsT>{
  typedef TrueClsT  NewTrueClsT;
  typedef typename PushFront<NewT, FalseClsT>::Result NewFalseClsT;
};

template<typename T1, typename T2>
struct Less{
	static const bool result = false;
};


template<typename Anchor, typename SeqT, typename RightSet, typename LeftSet>
struct PartitionImpl{};
template<typename Anchor, typename v1, typename RightSet, typename LeftSet>
struct PartitionImpl<Anchor, List<v1>, RightSet, LeftSet>{
  static const bool less=Less<v1, Anchor>::result;
  typedef typename Classify<less, v1, RightSet, LeftSet>::NewTrueClsT  RstRightSet;
  typedef typename Classify<less, v1, RightSet, LeftSet>::NewFalseClsT  RstLeftSet;  
};

template<typename RightSet, typename LeftSet, typename Anchor, typename v1, typename...vs>
struct PartitionImpl<Anchor, List<v1, vs...>, RightSet, LeftSet>{
  static const bool less=Less<typename PopFront<List<v1, vs...> >::Result, Anchor>::result;
  typedef typename Classify<less, v1, RightSet, LeftSet>::NewTrueClsT  TmpRstRightSet;
  typedef typename Classify<less, v1, RightSet, LeftSet>::NewFalseClsT  TmpRstLeftSet;

  typedef typename PartitionImpl<Anchor, List<vs...>, TmpRstRightSet, TmpRstLeftSet>::RstRightSet RstRightSet;
  typedef typename PartitionImpl<Anchor, List<vs...>, TmpRstRightSet, TmpRstLeftSet>::RstLeftSet  RstLeftSet;
};


template<typename T>
struct Partition{
};
template<typename v1, typename v2, typename...vs>
struct Partition<List<v1, v2, vs...> >{
  typedef v1 Anchor;
  typedef List<> RightSet;
  typedef List<> LeftSet;
  typedef typename PartitionImpl<Anchor, List<v1, v2, vs...>, RightSet, LeftSet>::RstRightSet  RstRightSet;
  typedef typename PartitionImpl<Anchor, List<v1, v2, vs...>, RightSet, LeftSet>::RstLeftSet   RstLeftSet;
};

//why introduce this? refer to Sort
template<typename SrcT, typename RightSet, typename LeftSet, template<typename > class SortOp>
struct SortSub{  
  typedef typename SortOp<RightSet>::Result  TmpRightSet2;
  typedef typename SortOp<LeftSet>::Result   TmpLeftSet2;
};
template<typename SrcT, typename LeftSet, template<typename> class SortOp>
struct SortSub<SrcT, SrcT, LeftSet, SortOp>{
  typedef SrcT  TmpRightSet2;
  typedef typename SortOp<LeftSet>::Result   TmpLeftSet2;
};
template<typename SrcT, typename RightSet, template<typename> class SortOp>
struct SortSub<SrcT, RightSet, SrcT, SortOp>{
  typedef typename SortOp<RightSet>::Result  TmpRightSet2;
  typedef SrcT   TmpLeftSet2;
};

template<typename T>
struct Sort;
template<>
struct Sort<List<> >{
  typedef List<> Result;
};
template<typename v1>
struct Sort<List<v1>> {
  typedef List<v1> Result;
};
template<typename v1, typename...vs>
struct Sort< List<v1, vs...> >{
  typedef List<v1, vs...> SrcType;
  typedef typename Partition< List<v1, vs...> >::RstRightSet TmpRightSet;
  typedef typename Partition< List<v1, vs...> >::RstLeftSet TmpLeftSet;

  //to by pass the case SrcType <==> TmpRightSet or  SrcType <==> TmpLeftSet
  typedef typename SortSub<SrcType, TmpRightSet, TmpLeftSet, Sort>::TmpRightSet2  TmpRightSet2;
  typedef typename SortSub<SrcType, TmpRightSet, TmpLeftSet, Sort>::TmpLeftSet2   TmpLeftSet2;

  typedef typename CatList<TmpRightSet2, TmpLeftSet2>::Result Result;
};

// This is test case
// It is f-ng huge and seems to be overkill
// Needs to be  extracted in a separate test

class A {};
class B {};
class C {};
class D {};
class E {};

template<>
struct Less<A, B> {
	static const bool result = true;
};
template<>
struct Less<A, C> {
	static const bool result = true;
};
template<>
struct Less<A, D> {
	static const bool result = true;
};
template<>
struct Less<A, E> {
	static const bool result = true;
};
template<>
struct Less<B, A> {
	static const bool result = false;
};
template<>
struct Less<B, C> {
	static const bool result = true;
};
template<>
struct Less<B, D> {
	static const bool result = true;
};
template<>
struct Less<B, E> {
	static const bool result = true;
};
template<>
struct Less<C, A> {
	static const bool result = false;
};
template<>
struct Less<C, B> {
	static const bool result = false;
};
template<>
struct Less<C, D> {
	static const bool result = true;
};
template<>
struct Less<C, E> {
	static const bool result = true;
};
template<>
struct Less<D, A> {
	static const bool result = false;
};
template<>
struct Less<D, B> {
	static const bool result = false;
};
template<>
struct Less<D, C> {
	static const bool result = false;
};
template<>
struct Less<D, E> {
	static const bool result = true;
};
template<>
struct Less<E, A> {
	static const bool result = false;
};
template<>
struct Less<E, B> {
	static const bool result = false;
};
template<>
struct Less<E, C> {
	static const bool result = false;
};
template<>
struct Less<E, D> {
	static const bool result = false;
};

// FIXME need to extract to separate 
int main() {
  using namespace std;
{
  typedef List<A> t;
  dumpListType(t());
  dumpListType(Sort<t>::Result());
  cout << endl;
}
{
  typedef List<B, A> t;
  dumpListType(t());
  dumpListType(Sort<t>::Result());
  cout << endl;
}
{
  //
  typedef List<B, B, A> t;
  dumpListType(t());
  dumpListType(Sort<t>::Result());
  cout << endl;
}
{
  typedef List<B, A, B > t;
  dumpListType(t());
  dumpListType(Sort<t>::Result());
  cout << endl;
}
{
  typedef List<B, B, B, B, B, A, B, B, B, B> t;
  dumpListType(t());
  dumpListType(Sort<t>::Result());
  cout << endl;
}
{
  typedef List<B, B, B, B, B, A> t;
  dumpListType(t());
  dumpListType(Sort<t>::Result());
  cout << endl;
}
{
  typedef List<E, E, E, D, D, C, B, B, B, A, A, A, A> t;
  dumpListType(t());
  dumpListType(Sort<t>::Result());
  cout << endl;
}
{
  typedef List<D, B, C, E, A, B, D, C> t;
  dumpListType(t());
  dumpListType(Sort<t>::Result());
  cout << endl; // this result is actuall incorrect
}
}
