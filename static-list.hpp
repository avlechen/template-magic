/*
 *  Created by avlechen (c), 2017
 *  MIT license, so do whatever you want.
 *  Mainly was inspired by Loki. 
 *  http://loki-lib.sourceforge.net/
 *  Some stuff is heavily reritten, some stuff may be new
 *
 *  This header specifies the List of types and following modificators:
 * 
 *  1. CatList
 *  2. PushFront
 *  3. PopFront
 *  4. Erase
 *  5. EraseAt
 *  6. IndexOf
 *  7. IsEmpty
 *
 *  CatList concatenate two lists, the rest is self-explanatory.
 */
#include <iostream>
#include <limits>

template<typename... vs>
struct List
{}; 
template<typename v1, typename...vs>
struct List<v1, vs...>{
};

template<typename T1, typename T2>
struct CatList{};
template<typename...v, typename...us>
struct CatList<List<v...>, List<us...> >{
  typedef List< v..., us... >  Result;
};

template<typename newT, typename srcT>
struct PushFront;

template<typename vadded, typename...vs>
struct PushFront<vadded, List<vs...> > {
  typedef List<vadded, vs...> Result;
};

template<typename T>
struct PopFront
{};

template<typename T>
struct PopFront<List<T> >
{
    typedef T Result;
    typedef List<> RemaindType;
};

template<typename v1, typename...vs>
struct PopFront<List<v1, vs...> >{
  typedef List<vs...>   RemaindType;
  typedef v1            Result;
};

template <typename TList, typename T> struct Erase
{};

template <typename T> // Do nothing
struct Erase<List<>, T>
{
    typedef List<> Result;
};

template <typename T> // Do nothing
struct Erase<List<T>, T>
{
    typedef List<> Result;
};

template <class Node, typename...Nodes> // Erase from head. Only one occurence will be deleted.
struct Erase<List<Node, Nodes...>, Node>
{
    typedef List<Nodes...> Result;
};

template <class Node, class OtherNode, typename...Nodes> // Erase from head. Only one occurence will be deleted.
struct Erase<List<OtherNode, Nodes...>, Node>
{
    typedef typename PushFront<OtherNode, typename Erase<List<Nodes...>, Node>::Result>::Result Result;
};

template<size_t I, typename L>
struct EraseAt;

template<size_t I>
struct EraseAt<I, List<> > {
  typedef List<> Result;
};

template<size_t I, typename Head, typename... Tail>
struct EraseAt<I, List<Head, Tail...>> {
  typedef typename PushFront<Head, typename EraseAt<I-1, List<Tail...>>::Result>::Result Result;
};

template<typename Head, typename... Tail>
struct EraseAt<0, List<Head, Tail...>> {
  typedef List<Tail...> Result;
};

template <typename N, typename L>
struct IndexOf {
};

template <typename Node, typename Head, typename... Tail>
struct IndexOf<Node, List<Head, Tail...>> {
  static const size_t value = IndexOf<Node, List<Tail...>>::value != std::numeric_limits<size_t>::max() ?
    IndexOf<Node, List<Tail...>>::value + 1 : std::numeric_limits<size_t>::max();
};

template <typename Node>
struct IndexOf<Node, List<> > {
  static const size_t value = std::numeric_limits<size_t>::max();
};

template <typename Node, typename... Tail>
struct IndexOf<Node, List<Node, Tail...> > {
  static const size_t value = 0;
};

template<typename T>
struct IsEmpty;

template<>
struct IsEmpty<List<>>{
  static const bool value = true;
};

template<typename... Ts>
struct IsEmpty<List<Ts...>>{
  static const bool value = false;
};

template<typename v1, typename...vs>
void dumpListTypeImpl(List<v1, vs...> ){
  std::cout << typeid(v1()).name () << " ";
  dumpListTypeImpl( List<vs...>() );
}

void dumpListTypeImpl(List<> ){
}

template<typename v1>
void dumpListTypeImpl(v1& v ){
  std::cout<<typeid(v).name();
}
template<typename...vs>
void dumpListType(List<vs...> ){
  std::cout << "Seq type < ";
  dumpListTypeImpl( List<vs...>() );
  std::cout << " >" << std::endl;
}
