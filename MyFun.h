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

//���ڱ��һ��sid��Ӧһ��signature�ı�
//struct EDGEFLAG
//{
//	unsigned int Sig;
//	unsigned int nSid;
//	unsigned int flag;
//};

//function declaration
 void Read(std::vector<EDGE> &edges, std::string &strPath);
 void InitialFind(SidMap &SidToSigMap, SidMap &SidToSigMapTmp, SigMap &SigToSidMap, SigMap &FSig_SidMap, std::set<unsigned int> &sidToZeroSig);
 void BipartiteMath(SidMap &FSig_SidMap, SidMap &SidToSigMapTmp, std::set<unsigned int> &sidToZeroSig);