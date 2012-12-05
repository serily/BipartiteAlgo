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


//find the sid only has one sig, store to SidtoOneSigMap
void FindSidToOneSig(SidMap &SidToSigMap,SidMap &SidToSigMapTmp, SigMap &SidtoOneSigMap, std::set<unsigned int> &sidToZeroSig)
{
	std::set<SIGNATURE> sigSet;
	for (SidMap::iterator it = SidToSigMap.begin(); it != SidToSigMap.end(); ++it)
	{
		if (it->second.size() == 1)
		{
			unsigned int n = *(it->second.begin());
			SidtoOneSigMap[n].insert(it->first);
			sigSet.insert(n);
			SidToSigMapTmp.erase(it->first);
		}
	}
	for (std::set<SIGNATURE>::iterator i = sigSet.begin(); i != sigSet.end(); ++i)
	{
		for (SidMap::iterator j = SidToSigMapTmp.begin(); j != SidToSigMapTmp.end(); ++j)
		{
			if (j->second.count(*i) != 0)
			{
				j->second.erase(*i);
			}
			if (j->second.size() == 0)
			{
				sidToZeroSig.insert(j->first);
			}
		}
	}
}