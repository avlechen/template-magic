/*
 *  Created by avlechen (c), 2017 
 *  MIT license, so do whatever you want.
 *
 *  This code is based on one stackoverflow answer.
 *  I've lost a link to  the original post, so if you have one, please submit url.
 * 
 *  Here you may find an example of compile-time sorting of integer values
 */

#include <iostream>

using namespace std;

template<int... vs>
struct Seq
{}; 
template<int v1, int...vs>
struct Seq<v1, vs...>{
};


template<typename newT, typename srcT>
struct PushFront{
};
template<int vadded, int...vs>
struct PushFront<Seq<vadded>, Seq<vs...>>{
  typedef Seq<vadded, vs...> ResultType;
};

template<typename T>
struct PopFront{
};
template<int v1, int...vs>
struct PopFront<Seq<v1, vs...>>{
  typedef Seq<vs...> RemaindType;
  typedef Seq<v1>    ResultType;
};

template<typename T1, typename T2>
struct CatSeq{};
template<int...v, int...us>
struct CatSeq<Seq<v...>, Seq<us...>>{
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
struct Compare{};
template<int v1, int v2>
struct Compare<Seq<v1>, Seq<v2>>{
  static const bool result=(v1<v2); 
};


template<typename AnchorT, typename SeqT, typename GESet, typename LSet>
struct PartitionImpl{};
template<typename GESet, typename LSet, int anchorv, int v1>
struct PartitionImpl<Seq<anchorv>, Seq<v1>, GESet, LSet>{
  static const bool isge=Compare<typename PopFront<Seq<v1>>::ResultType, Seq<anchorv>>::result;
  typedef typename Classify<isge, Seq<v1>, GESet, LSet>::NewTrueClsT  RstGESet;
  typedef typename Classify<isge, Seq<v1>, GESet, LSet>::NewFalseClsT  RstLSet;  
};

template<typename GESet, typename LSet, int anchorv, int v1, int...vs>
struct PartitionImpl<Seq<anchorv>, Seq<v1, vs...>, GESet, LSet>{
  static const bool isge=Compare<typename PopFront<Seq<v1, vs...>>::ResultType, Seq<anchorv>>::result;
  typedef typename Classify<isge, Seq<v1>, GESet, LSet>::NewTrueClsT  TmpRstGESet;
  typedef typename Classify<isge, Seq<v1>, GESet, LSet>::NewFalseClsT  TmpRstLSet;

  typedef typename PartitionImpl<Seq<anchorv>, Seq<vs...>, TmpRstGESet, TmpRstLSet>::RstGESet RstGESet;
  typedef typename PartitionImpl<Seq<anchorv>, Seq<vs...>, TmpRstGESet, TmpRstLSet>::RstLSet  RstLSet;
};


template<typename T>
struct Partition{
};
template<int v1, int v2, int...vs>
struct Partition<Seq<v1, v2, vs...>>{
  typedef Seq<v1> AnchorType;
  typedef Seq<> GESet;
  typedef Seq<> LSet;
  typedef typename PartitionImpl<AnchorType, Seq<v1, v2, vs...>, GESet, LSet>::RstGESet  RstGESet;
  typedef typename PartitionImpl<AnchorType, Seq<v1, v2, vs...>, GESet, LSet>::RstLSet   RstLSet;
};

//why introduce this? refer to Sort
template<typename SrcT, typename GESet, typename LSet, template<typename > class SortOp>
struct SortSub{  
  typedef typename SortOp<GESet>::ResultType  TmpGESet2;
  typedef typename SortOp<LSet>::ResultType   TmpLSet2;
};
template<typename SrcT, typename LSet, template<typename> class SortOp>
struct SortSub<SrcT, SrcT, LSet, SortOp>{
  typedef SrcT  TmpGESet2;
  typedef typename SortOp<LSet>::ResultType   TmpLSet2;
};
template<typename SrcT, typename GESet, template<typename> class SortOp>
struct SortSub<SrcT, GESet, SrcT, SortOp>{
  typedef typename SortOp<GESet>::ResultType  TmpGESet2;
  typedef SrcT   TmpLSet2;
};

template<typename T>
struct Sort;
template<>
struct Sort<Seq<>>{
  typedef Seq<> ResultType;
};
template<int v>
struct Sort< Seq<v> >{
  typedef Seq<v> ResultType;
};
template<int v1, int...vs>
struct Sort< Seq<v1, vs...> >{
  typedef Seq<v1, vs...> SrcType;
  typedef typename Partition< Seq<v1, vs...> >::RstGESet TmpGESet;
  typedef typename Partition< Seq<v1, vs...> >::RstLSet TmpLSet;

  //to by pass the case SrcType <==> TmpGESet or  SrcType <==> TmpLSet
  typedef typename SortSub<SrcType, TmpGESet, TmpLSet, Sort>::TmpGESet2  TmpGESet2;
  typedef typename SortSub<SrcType, TmpGESet, TmpLSet, Sort>::TmpLSet2   TmpLSet2;

  typedef typename CatSeq<TmpGESet2, TmpLSet2>::ResultType ResultType;
};


void dumpSeqTypeImpl(Seq<> ){
}
template<int v1>
void dumpSeqTypeImpl(Seq<v1> ){
  cout<<v1<<" ";
}
template<int v1, int...vs>
void dumpSeqTypeImpl(Seq<v1, vs...> ){
  cout<<v1<<" ";
  dumpSeqTypeImpl( Seq<vs...>() );
}
template<int...vs>
void dumpSeqType(Seq<vs...> ){
  cout<<"Seq type < ";
  dumpSeqTypeImpl( Seq<vs...>() );
  cout<<" >"<<endl;
}

// FIXME: This should be extracted to a separate test cases.
// FIXME: get some light-weight unit test lib
int main(){
#if 1
  typedef Seq<4, 7, 3, 9, 1, 2, 5, 5, 19, 5> TestType;
  TestType s3;
  typename Partition< TestType >::RstGESet ts1;
  typename Partition< TestType >::RstLSet ts2;

  typename Sort<TestType>::ResultType so1;
  dumpSeqType( so1 );
#endif 

#if 0
  typedef Seq<TEST_DATA_100> TAdvanceType;
  typename Sort<TAdvanceType>::ResultType soadvance;
  dumpSeqType(soadvance);
#endif

  return 0;
}
