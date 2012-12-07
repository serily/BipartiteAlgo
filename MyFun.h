#pragma once
#include "stdafx.h"

#ifndef SIGNATURE
#define SIGNATURE unsigned int
#endif

#ifndef SNORTID
#define SNORTID unsigned int
#endif

typedef std::map<SIGNATURE, std::set<SNORTID>> SigMap;
typedef std::map<SNORTID, std::set<SIGNATURE>> SidMap;



struct EDGE
{
	unsigned int Sig;
	unsigned int nSid;
};

//用于标记一个sid对应一个signature的边
//struct EDGEFLAG
//{
//	unsigned int Sig;
//	unsigned int nSid;
//	unsigned int flag;
//};

//function declaration
 void Read(std::vector<EDGE> &edges, std::string &strPath);
 void InitialFind(SidMap &SidToSigMap, SidMap &SidToSigMapTmp, SigMap &SigToSidMap, SigMap &FSig_SidMap, std::set<SNORTID> &sidToZeroSig);
 void BipartiteMath(SidMap &FSig_SidMap, SidMap &SidToSigMapTmp, std::set<SNORTID> &sidToZeroSig);
 void InsertRemain(SigMap &FSig_SidMap, SidMap &SidToSigMap, SigMap &SigToSidMap, std::set<SNORTID> &sidToZeroSig);