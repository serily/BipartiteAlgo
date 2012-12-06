#include "stdafx.h"
#include "MyFun.h"

void Read(std::vector<EDGE> &edges, std::string &strPath)
{
	std::ifstream fin(strPath, std::ios::binary);
	size_t nCnt = 0;
	fin.read((char*)&nCnt, 4);
	EDGE edge;
	for (size_t i = 0; i < nCnt; ++i)
	{
		fin.read((char*)&edge.Sig, 4);
		fin.read((char*)&edge.nSid, 4);
		edges.push_back(edge);
	}
}


//First Step: find sids only has one sig, store to SidtoOneSigMap
void InitialFind(SidMap &SidToSigMap,SidMap &SidToSigMapTmp, SigMap &SigToSidMap,
				 SigMap &FSig_SidMap, std::set<unsigned int> &sidToZeroSig)
{
	std::set<SIGNATURE> sigSet;
	std::set<SNORTID> sidSet;

	//sid only to one sig
	for (SidMap::iterator it = SidToSigMap.begin(); it != SidToSigMap.end(); ++it)
	{
		if (it->second.size() == 1)
		{
			unsigned int n = *(it->second.begin());
			FSig_SidMap[n].insert(it->first);
			sigSet.insert(n);
			SidToSigMapTmp.erase(it->first);
		}
	}

	//sig only to one sid
	for (SigMap::iterator it = SigToSidMap.begin(); it != SigToSidMap.end(); ++it)
	{
		if (it->second.size() == 1)
		{
			unsigned int sid = *(it->second.begin());
			if (sidSet.count(sid) == 0)
			{
				FSig_SidMap[it->first].insert(sid);
				sidSet.insert(sid);
				SidToSigMapTmp.erase(sid);
			}
		}
	}

	for (std::set<SIGNATURE>::iterator i = sigSet.begin(); i != sigSet.end(); ++i)
	{
		for (SidMap::iterator j = SidToSigMapTmp.begin(); j != SidToSigMapTmp.end();)
		{
			if (j->second.count(*i) != 0)
			{
				j->second.erase(*i);
				if (j->second.size() == 0)
				{
					sidToZeroSig.insert(j->first);
					j = SidToSigMapTmp.erase(j);
					continue;
				}
			}
			++j;
		}
	}
}

//Second Step: BipartiteMathing
 void BipartiteMath(SidMap &BSigToSidMap, SidMap &SidToSigMapTmp, std::set<unsigned int> &sidToZeroSig)
 {
	struct EDGEFLAG
	{
		unsigned int Sig;
		unsigned int nSid;
		unsigned int flag;
	};


 }