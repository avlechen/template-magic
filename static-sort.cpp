#include <iostream>
#include <typeinfo>

using namespace std;

template<typename... vs>
struct Seq
{}; 
template<typename v1, typename...vs>
struct Seq<v1, vs...>{
};


template<typename newT, typename srcT>
struct PushFront{
};
template<typename vadded, typename...vs>
struct PushFront<vadded, Seq<vs...> >{
  typedef Seq<vadded, vs...> ResultType;
};

template<typename T>
struct PopFront{
};
template<typename v1, typename...vs>
struct PopFront<Seq<v1, vs...> >{
  typedef Seq<vs...> RemaindType;
  typedef v1    ResultType;
};

template<typename T1, typename T2>
struct CatSeq{};
template<typename...v, typename...us>
struct CatSeq<Seq<v...>, Seq<us...> >{
  typedef Seq< v..., us... >  ResultType;
};


template<bool c, typename NewT, typename TrueClsT, typename FalseClsT>
struct Classify{
};
template<typename NewT, typename TrueClsT, typename FalseClsT>
struct Classify<true, NewT, TrueClsT, FalseClsT>{
  typedef typename PushFront<NewT, TrueClsT>::ResultType NewTrueClsT;
  typedef FalseClsT  NewFalseClsT;
};
template<typename NewT, typename TrueClsT, typename FalseClsT>
struct Classify<false, NewT, TrueClsT, FalseClsT>{
  typedef TrueClsT  NewTrueClsT;
  typedef typename PushFront<NewT, FalseClsT>::ResultType NewFalseClsT;
};

template<typename T1, typename T2>
struct Less{
	static const bool result = false;
};


template<typename Anchor, typename SeqT, typename RightSet, typename LeftSet>
struct PartitionImpl{};
template<typename Anchor, typename v1, typename RightSet, typename LeftSet>
struct PartitionImpl<Anchor, Seq<v1>, RightSet, LeftSet>{
  static const bool less=Less<v1, Anchor>::result;
  typedef typename Classify<less, v1, RightSet, LeftSet>::NewTrueClsT  RstRightSet;
  typedef typename Classify<less, v1, RightSet, LeftSet>::NewFalseClsT  RstLeftSet;  
};

template<typename RightSet, typename LeftSet, typename Anchor, typename v1, typename...vs>
struct PartitionImpl<Anchor, Seq<v1, vs...>, RightSet, LeftSet>{
  static const bool less=Less<typename PopFront<Seq<v1, vs...> >::ResultType, Anchor>::result;
  typedef typename Classify<less, v1, RightSet, LeftSet>::NewTrueClsT  TmpRstRightSet;
  typedef typename Classify<less, v1, RightSet, LeftSet>::NewFalseClsT  TmpRstLeftSet;

  typedef typename PartitionImpl<Anchor, Seq<vs...>, TmpRstRightSet, TmpRstLeftSet>::RstRightSet RstRightSet;
  typedef typename PartitionImpl<Anchor, Seq<vs...>, TmpRstRightSet, TmpRstLeftSet>::RstLeftSet  RstLeftSet;
};


template<typename T>
struct Partition{
};
template<typename v1, typename v2, typename...vs>
struct Partition<Seq<v1, v2, vs...> >{
  typedef v1 Anchor;
  typedef Seq<> RightSet;
  typedef Seq<> LeftSet;
  typedef typename PartitionImpl<Anchor, Seq<v1, v2, vs...>, RightSet, LeftSet>::RstRightSet  RstRightSet;
  typedef typename PartitionImpl<Anchor, Seq<v1, v2, vs...>, RightSet, LeftSet>::RstLeftSet   RstLeftSet;
};

//why introduce this? refer to Sort
template<typename SrcT, typename RightSet, typename LeftSet, template<typename > class SortOp>
struct SortSub{  
  typedef typename SortOp<RightSet>::ResultType  TmpRightSet2;
  typedef typename SortOp<LeftSet>::ResultType   TmpLeftSet2;
};
template<typename SrcT, typename LeftSet, template<typename> class SortOp>
struct SortSub<SrcT, SrcT, LeftSet, SortOp>{
  typedef SrcT  TmpRightSet2;
  typedef typename SortOp<LeftSet>::ResultType   TmpLeftSet2;
};
template<typename SrcT, typename RightSet, template<typename> class SortOp>
struct SortSub<SrcT, RightSet, SrcT, SortOp>{
  typedef typename SortOp<RightSet>::ResultType  TmpRightSet2;
  typedef SrcT   TmpLeftSet2;
};

template<typename T>
struct Sort;
template<>
struct Sort<Seq<> >{
  typedef Seq<> ResultType;
};
template<typename v1>
struct Sort<Seq<v1>> {
  typedef Seq<v1> ResultType;
};
template<typename v1, typename...vs>
struct Sort< Seq<v1, vs...> >{
  typedef Seq<v1, vs...> SrcType;
  typedef typename Partition< Seq<v1, vs...> >::RstRightSet TmpRightSet;
  typedef typename Partition< Seq<v1, vs...> >::RstLeftSet TmpLeftSet;

  //to by pass the case SrcType <==> TmpRightSet or  SrcType <==> TmpLeftSet
  typedef typename SortSub<SrcType, TmpRightSet, TmpLeftSet, Sort>::TmpRightSet2  TmpRightSet2;
  typedef typename SortSub<SrcType, TmpRightSet, TmpLeftSet, Sort>::TmpLeftSet2   TmpLeftSet2;

  typedef typename CatSeq<TmpRightSet2, TmpLeftSet2>::ResultType ResultType;
};


void dumpSeqTypeImpl(Seq<> ){
}

template<typename v1>
void dumpSeqTypeImpl(v1& v ){
  cout<<typeid(v).name() << " ";
}

template<typename v1, typename...vs>
void dumpSeqTypeImpl(Seq<v1, vs...> ){
  cout<<typeid(v1()).name () << " ";
  dumpSeqTypeImpl( Seq<vs...>() );
}
template<typename...vs>
void dumpSeqType(Seq<vs...> ){
  cout<<"Seq type < ";
  dumpSeqTypeImpl( Seq<vs...>() );
  cout<<" >"<<endl;
}

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
int main(){
{
  typedef Seq<A> t;
  dumpSeqType(t());
  dumpSeqType(Sort<t>::ResultType());
  cout << endl;
}
{
  typedef Seq<B, A> t;
  dumpSeqType(t());
  dumpSeqType(Sort<t>::ResultType());
  cout << endl;
}
{
  //
  typedef Seq<B, B, A> t;
  dumpSeqType(t());
  dumpSeqType(Sort<t>::ResultType());
  cout << endl;
}
{
  typedef Seq<B, A, B > t;
  dumpSeqType(t());
  dumpSeqType(Sort<t>::ResultType());
  cout << endl;
}
{
  typedef Seq<B, B, B, B, B, A, B, B, B, B> t;
  dumpSeqType(t());
  dumpSeqType(Sort<t>::ResultType());
  cout << endl;
}
{
  typedef Seq<B, B, B, B, B, A> t;
  dumpSeqType(t());
  dumpSeqType(Sort<t>::ResultType());
  cout << endl;
}
{
  typedef Seq<E, E, E, D, D, C, B, B, B, A, A, A, A> t;
  dumpSeqType(t());
  dumpSeqType(Sort<t>::ResultType());
  cout << endl;
}
{
  typedef Seq<D, B, C, E, A, B, D, C> t;
  dumpSeqType(t());
  dumpSeqType(Sort<t>::ResultType());
  cout << endl; // this result is actuall incorrect
}
}
