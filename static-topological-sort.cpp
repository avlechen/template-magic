/*
 *  Created by avlechen (c), 2017
 *  MIT license, so do whatever you want.
 *  
 *  This one is implementation of Kahn's algorithm on templates.
 *  Each type represents a Node in a directed graph.
 *  Edges are dependencies between types and stored in a struct Dependencies
 * 
 */

#include <typeinfo>
#include "static-list.hpp"

// This may be seen as an edge in a directed graph.
// Node as a destination vertex and DList contains source vertecies
template <typename Node>
struct Dependencies {
  typedef List<> DList;
};

template<bool selector, typename TrueType, typename FalseType>
struct Select;

template<typename TrueType, typename FalseType>
struct Select<true, TrueType, FalseType>
{
  typedef TrueType Result;
};

template<typename TrueType, typename FalseType>
struct Select<false, TrueType, FalseType>
{
  typedef FalseType Result;
};

template<typename Node, typename DepsSeq>
struct EraseDep
{};

template<typename Node>
struct EraseDep<Node, List<> >
{
  typedef List<> Result;
};

template<typename Node, typename Dep, typename... Deps>
struct EraseDep<Node, List<Dep, Deps...>>
{
  typedef typename PushFront<typename Erase<Dep, Node>::Result, typename EraseDep<Node, List<Deps...>>::Result >::Result Result;
};

template<typename SrcT, typename Deps>
struct FindNoDeps;

template<typename SrcT, typename Deps>
class FindNoDeps {
  typedef typename PopFront<Deps>::Result Dep;
  typedef typename PopFront<Deps>::RemaindType OtherDeps;

  typedef typename PopFront<SrcT>::Result Result;
  typedef typename PopFront<SrcT>::RemaindType OtherSrcT;

  // if Dep is empty, required node will be defined in Selected Type with empty list in SelectedDeps
  // Otherwise we will get the remainders of both lists to look into
  typedef typename Select<IsEmpty<Dep>::value, Result, OtherSrcT>::Result SelectedTypes;
  typedef typename Select<IsEmpty<Dep>::value, Dep, OtherDeps>::Result SelectedDeps;
public:
  typedef typename FindNoDeps<SelectedTypes, SelectedDeps>::Node Node;
};

template<typename SrcT>
class FindNoDeps<SrcT, List<>>
{
public:
  typedef SrcT Node;
};

template<typename SrcT, typename Deps>
struct TopSortImpl
{
  typedef typename FindNoDeps<SrcT, Deps>::Node Node;
  typedef typename Erase<SrcT, Node>::Result RemainNodes;
  typedef typename EraseAt<IndexOf<Node, SrcT>::value, Deps>::Result RemainDependencies;
  typedef typename EraseDep<Node, RemainDependencies>::Result UpdatedLeftDependencies;
  typedef typename TopSortImpl<RemainNodes, UpdatedLeftDependencies>::Result PrevResult;

  typedef typename PushFront<Node, PrevResult>::Result Result;
};

template<typename T>
struct TopSortImpl<List<T>, List<>>
{
  typedef List<T> Result;
};

template<>
struct TopSortImpl<List<>, List<> >
{
  typedef List<> Result;
};

template<typename SrcT>
struct MakeDepsList;

template<>
struct MakeDepsList<List<> > {
  typedef List<> Result;
};

template<typename Node, typename... Nodes>
struct MakeDepsList<List<Node, Nodes...>>{
  typedef typename PushFront<typename Dependencies<Node>::DList, typename MakeDepsList<List<Nodes...> >::Result>::Result Result;
};


template<typename T>
struct TopSort;
template<>
struct TopSort<List<> > {
  typedef List<> Result;
};
template<typename... Nodes>
struct TopSort< List<Nodes...> > {
  typedef List<Nodes...> SrcType;
  typedef typename MakeDepsList<SrcType>::Result Deps;

  typedef typename TopSortImpl<SrcType, Deps>::Result Result;
};

// Working example with test

class A {};
class B {};
class C {};
class D {};
class E {};

template <>
struct Dependencies<B> {
  typedef List<A> DList;
};
template <>
struct Dependencies<C> {
  typedef List<A> DList;
};
template <>
struct Dependencies<D> {
  typedef List<A, B, C> DList;
};
template <>
struct Dependencies<E> {
  typedef List<C, D> DList;
};

int main()
{
  typedef List<C, D, E, A, B> t;
  dumpListType(t());
  dumpListType(TopSort<t>::Result());
  std::cout << std::endl;
}