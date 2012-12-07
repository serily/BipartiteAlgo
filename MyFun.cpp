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

bool dfs(size_t &v, size_t &rNum, std::vector<std::vector<size_t>> &map, std::vector<size_t> &match, std::vector<size_t> &vis)	
{	
	if (v)
	{
		/** 枚举右边的点 */	
		for(size_t i = 1; i <= rNum; i++)	
		{	
			/** 如果这次增广路寻找右边的这个点没被访问，且与左边的v连通 */	
			/*考察与左边的点连通的所有右边的点*/
			if(!vis[i - 1] && map[v - 1][i - 1])	
			{	
				vis[i - 1] = 1;	
 	
				/**	
				 * dfs(match[i], n)：右边的i点的匹配顶点为match[i]	
				 * 说明这条边是匹配边，那么我们往下搜看看能不能完成增广路	
				 * 如果能完成增广路，则我们将i点的匹配顶点修改为v	
				 * 如果match[i]为0则说明i点是未被匹配的点	
				 * 那么将i点的匹配顶点修改为v	
				 * 这段代码完成了“匹配->未匹配”交替搜索	
				 */	
				if(dfs(match[i - 1], rNum, map, match, vis) || !match[i - 1])	
				{	
					match[i - 1] = v;	
					return true;	
				}	
			}	
		}	
	}
 	
    return false;	
}	
void max_match(size_t &lNum, size_t &rNum, std::vector<std::vector<size_t>> &map, std::vector<size_t> &match, std::vector<size_t> &vis)	
{	
	fill(match.begin(), match.end(), 0);
    /** 枚举左边的点 */	
    for(size_t i = 1; i <= lNum; i++)	
    {	
        /** 重新初始化vis */	
		fill(vis.begin(), vis.end(), 0);
 	
        /** 对左边的i点搜增广路 */	
        dfs(i, rNum, map, match, vis);	
    }	
}
//Second Step: BipartiteMathing
 void BipartiteMath(SidMap &FSig_SidMap, SidMap &SidToSigMapTmp, std::set<unsigned int> &sidToZeroSig)
 {
	size_t lNum = 0;
	size_t rNum = 0;
	std::map<SNORTID, unsigned int> mSidInt;//sid与map中整数的对应关系
	std::map<SIGNATURE, unsigned int> mSigInt;//sig与map中整数的对应关系
	std::set<SIGNATURE> sigTmpSet;
	std::set<SNORTID> sidTmpSet;
	//将SidToSigMapTmp中的sid与sig的对应关系存在临时mapTmp中
	for (SidMap::iterator it = SidToSigMapTmp.begin(); it != SidToSigMapTmp.end(); ++it)
	{
		sidTmpSet.insert(it->first);
		for (std::set<SIGNATURE>::iterator j = it->second.begin(); j != it->second.end(); ++j)
		{
			sigTmpSet.insert(*j);
		}
	}

	lNum = sidTmpSet.size();
	size_t sidCnt = 1;
	for (std::set<SNORTID>::iterator it = sidTmpSet.begin(); it != sidTmpSet.end(); ++it)
	{
		mSidInt[*it] = sidCnt;
		++sidCnt;
	}
	//sid to int
	//std::ofstream fsid("C:\\test\\sid_int.txt");
	//for (std::map<SNORTID, unsigned int>::iterator it = mSidInt.begin(); it != mSidInt.end(); ++it)
	//{
	//	fsid << it->first << "\t" << it->second;
	//	fsid << std::endl;
	//}
	//fsid.close();

	rNum = sigTmpSet.size();
	size_t sigCnt = 1;
	for (std::set<SIGNATURE>::iterator j = sigTmpSet.begin(); j != sigTmpSet.end(); ++j)
	{
		mSigInt[*j] = sigCnt;
		++sigCnt;
	}
	//sig to int
	//std::ofstream fsig("C:\\test\\sig_int.txt");
	//for (std::map<SIGNATURE, unsigned int>::iterator it = mSigInt.begin(); it != mSigInt.end(); ++it)
	//{
	//	fsig << it->first << "\t" << it->second;
	//	fsig << std::endl;
	//}
	//fsig.close();


	std::vector<std::vector<size_t>> map(lNum, std::vector<size_t>(rNum));
	for (size_t i = 0; i < lNum; ++i)
	{
		for (size_t j = 0; j < rNum; ++j)
		{
			map[i][j] = 0;
		}
	}

	for (SidMap::iterator i = SidToSigMapTmp.begin(); i != SidToSigMapTmp.end(); ++i)
	{
		unsigned int sid = (mSidInt.find(i->first))->second;//sid对应的整型编码
		for (std::set<SIGNATURE>::iterator j = i->second.begin(); j != i->second.end(); ++j)
		{
			unsigned int sig = (mSigInt.find(*j))->second;//sig对应的整型编码
			map[sid-1][sig-1] = 1;
		}
	}

	//std::ofstream f1("C:\\test\\testmap.txt");
	//for (size_t i = 0; i < lNum; ++i)
	//{
	//	for (size_t j = 0; j < rNum; ++j)
	//	{
	//		f1 << map[i][j] << " ";
	//	}
	//	f1 << std::endl;
	//}
	//f1.close();

	//进行最大二分匹配
	std::vector<size_t> vis(rNum);
	std::vector<size_t> match(rNum);
	max_match(lNum, rNum, map, match, vis);

	//match result
	//std::ofstream f2("C:\\test\\match.txt");
	//for (size_t i = 0; i < rNum; ++i)
	//{
	//	f2 << match[i] << std::endl;
	//}
	//f2.close();

	std::map<unsigned int, SNORTID> mIntSid;//整型编码与sid的对应关系
	std::map<unsigned int, SIGNATURE> mIntSig;//整型编码与sig的对应关系
	for (std::map<SNORTID, unsigned int>::iterator it = mSidInt.begin(); it != mSidInt.end(); ++it)
	{
		mIntSid[it->second] = it->first;
	}
	for (std::map<SIGNATURE, unsigned int>::iterator it = mSigInt.begin(); it != mSigInt.end(); ++it)
	{
		mIntSig[it->second] = it->first;
	}

	//将二分匹配的结果放入FSig_SidMap中
	size_t matchCnt = 0;
	for (size_t i = 1; i <= rNum; ++i)
	{
		if (match[i - 1])
		{
			++matchCnt;
			unsigned int sidInt = match[i-1];
			unsigned int sigInt = i;
			unsigned int sid = (mIntSid.find(sidInt))->second;
			unsigned int sig = (mIntSig.find(sigInt))->second;
			FSig_SidMap[sig].insert(sid);
			SidToSigMapTmp.erase(sid);
		}
	}

	std::cout << std::endl << "最大二分匹配找到了" << matchCnt << "个sid与sig一对一关系.还剩" << SidToSigMapTmp.size() 
		<< "个sid未分配" << std::endl;

	//将剩余的未分配的sid放入sidToZeroSig中
	for (SigMap::iterator it = SidToSigMapTmp.begin(); it != SidToSigMapTmp.end(); ++it)
	{
		sidToZeroSig.insert(it->first);
	}
 }