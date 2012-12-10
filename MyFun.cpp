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
				 SigMap &FSig_SidMap, std::set<SNORTID> &sidToZeroSig)
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
 void BipartiteMath(SidMap &FSig_SidMap, SidMap &SidToSigMapTmp, std::set<SNORTID> &sidToZeroSig)
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

void OptimizeInsert(std::set<SIGNATURE> &sigSet, std::map<SNORTID, std::set<SIGNATURE>> &sidToZeroSigMapTMP, 
					SigMap &FSig_SidMap)
{
	 std::map<SIGNATURE, size_t> sigTosidCnt;//已经分配的sig对应的sid个数
	 for (std::set<SIGNATURE>::iterator it = sigSet.begin(); it != sigSet.end(); ++it)
	 {
		 size_t nCnt = (FSig_SidMap.find(*it))->second.size();
		 sigTosidCnt[*it] = nCnt;
	 }

	 std::map<SNORTID, SIGNATURE> mapTmp;//sid对应sig的临时map
	 SigMap mapTmpSet;//sig对应sid集合的临时map
	 bool flag = true;//是否进行了调整
	 while (flag)
	 {
		 flag = false;
		 for (SidMap::iterator it = sidToZeroSigMapTMP.begin(); it != sidToZeroSigMapTMP.end(); ++it)
		 {
			 if (mapTmp.count(it->first) == 0)//若该sid还没有分配signature,就把当前与它相关的具有最少sid数的signature分配给它
			 {
				 SIGNATURE sig;
				 size_t min = SIZE_MAX;
				 for (std::set<SIGNATURE>::iterator j = it->second.begin(); j != it->second.end(); ++j)
				 {
					 if (min > (sigTosidCnt.find(*j))->second)
					 {
						 min = (sigTosidCnt.find(*j))->second;
						 sig = *j;
					 }
				 }
				 mapTmp[it->first] = sig;
				 mapTmpSet[sig].insert(it->first);
				 ++(sigTosidCnt.find(sig))->second;
				 flag = true;
			 }
			 else//该sid已经分配，则看是否能进行调整
			 {
				 SIGNATURE assignSig = mapTmp[it->first];//此sid分配的sig
				 size_t num = (sigTosidCnt.find(assignSig))->second;//assginSig当前具有的sid个数
				 size_t min = SIZE_MAX;//该sid对应的当前具有最小个数sid的sig
				 SIGNATURE sig;//
				 for (std::set<SIGNATURE>::iterator j = it->second.begin(); j != it->second.end(); ++j)
				 {
					 if (min > (sigTosidCnt.find(*j))->second)
					 {
						 min = (sigTosidCnt.find(*j))->second;
						 sig = *j;
					 }
				 }
				 if ((min + 1) < num)//将此sid直接分配给sig，解除它和assginSig的绑定关系
				 {
					 --(sigTosidCnt.find(assignSig))->second;
					 mapTmp[it->first] = sig;
					 mapTmpSet[sig].insert(it->first);
					 ++(sigTosidCnt.find(sig))->second;
					 flag = true;
				 }
				 //考察sig对应的所有sid，这些sid中，sid2对应的具有最少sid个数的sig2,如果这个sig2对应的的sid个数比min小，
				 //则把sid2分给sig2,然后把sid分给sig。即郝伟臣方法的第四步。不包含这一步的结果存于Laststep3.txt，包含的存于LastStep4.txt中
				 else if ((min + 1) == num)
				 {
					 SIGNATURE sigFirstLevel, sigSecondLevel;
					 SNORTID sidSecond;
					 size_t secondMin = min;
					 bool change = false;

					 std::set<SNORTID> sidCntTmp;//当前分配的sig对应的sidToZeroSig中的sid集合
					 for (std::map<SNORTID, SIGNATURE>::iterator j = mapTmp.begin(); j != mapTmp.end(); ++j)
					 {
						 if (j->second == sig)
						 {
							 sidCntTmp.insert(j->first);
						 }
					 }

					 for (std::set<SIGNATURE>::iterator j = it->second.begin(); j != it->second.end(); ++j)
					 {
						 if ((sigTosidCnt).find(*j)->second == min)
						 {
							 for (std::set<SNORTID>::iterator h = mapTmpSet[*j].begin(); h != mapTmpSet[*j].end(); ++h)//考察每个sid对应的sig
							 {
								 for (std::set<SIGNATURE>::iterator l = sidToZeroSigMapTMP[*h].begin(); l != sidToZeroSigMapTMP[*h].end();
									 ++l)
								 {
									 if ((sigTosidCnt.find(*l))->second < secondMin)
									 {
										 secondMin = (sigTosidCnt.find(*j))->second;
										 sigFirstLevel = *j;
										 sigSecondLevel = *l;
										 sidSecond = *h;
										 change = true;
									 }
								 }
							 }
						 }
					 }
					 if (change)
					 {
						 //将sidSecond解绑
						 SIGNATURE sigSecondTmp = mapTmp[sidSecond];
						 --(sigTosidCnt.find(sigSecondTmp))->second;
						 mapTmp[sidSecond] = sigSecondLevel;
						 ++(sigTosidCnt.find(sigSecondLevel))->second;
						 SIGNATURE sigFirstTmp = mapTmp[it->first];
						 --(sigTosidCnt.find(sigFirstTmp))->second;
						 mapTmp[it->first] = sigFirstLevel;
						 ++(sigTosidCnt.find(sigFirstLevel))->second;
					 }
				 }
			 }
		 }
	 }

	 //while (flag)
	 //{
		// flag = false;
		// for (std::map<SNORTID, std::set<SIGNATURE>>::iterator i = sidToZeroSigMapTMP.begin(); 
		//	 i != sidToZeroSigMapTMP.end(); ++i)
		// {
		//	 SIGNATURE sig;
		//	 size_t tmp = mapTmp.count(i->first);
		//	 bool mark = false;//该sid还未分配
		//	 if (tmp != 0)
		//	 {
		//		 mark = true;//该sid已经考察过了
		//		 sig = (mapTmp.find(i->first))->second;
		//	 }
		//	 else
		//	 {
		//		 sig = *(i->second.begin());
		//	 }
		//	size_t sigCnt = (sigTosidCnt.find(sig))->second;
		//	 for (std::set<SIGNATURE>::iterator j = i->second.begin(); j != i->second.end(); ++j)
		//	 {
		//		 size_t sigCnt2 = (sigTosidCnt.find(*j))->second;
		//		 if (mark)
		//		 {
		//			 if (sigCnt > (sigCnt2 + 1))
		//			 {
		//				 --(sigTosidCnt.find(sig))->second;
		//				 sig = *j;
		//			 }
		//		 }
		//		 else
		//		 {
		//			 if ((sigCnt + 1) > (sigCnt2 + 1))
		//			 {
		//				 sig = *j;
		//			 }
		//		 }
		//	 }
		//	 size_t sigInitCnt = mapTmp.count(i->first);
		//	 if (sigInitCnt == 0)//该sid还没有分配过
		//	 {
		//		 mapTmp[i->first] = sig;
		//		 ++(sigTosidCnt.find(sig))->second;
		//		 flag = true;
		//	 }
		//	 else
		//	 {
		//		 SIGNATURE sigInit = mapTmp[i->first];
		//		 if (sigInit != sig)//进行了调整
		//		 {
		//			 mapTmp[i->first] = sig;
		//			 ++(sigTosidCnt.find(sig))->second;
		//			 --(sigTosidCnt.find(sigInit))->second;
		//			 flag = true;
		//		 }
		//	 }
		// }
	 //}

	 //将mapTmp中放入FSig_SidMap中
	 for (std::map<SNORTID, SIGNATURE>::iterator it = mapTmp.begin(); it != mapTmp.end(); ++it)
	 {
		 FSig_SidMap[it->second].insert(it->first);
	 }
}

 void InsertRemain(SigMap &FSig_SidMap, SidMap &SidToSigMap, SigMap &SigToSidMap, std::set<SNORTID> &sidToZeroSig)
 {
	 SidMap sidToZeroSigMap;
	 SidMap sidToZeroSigMapTMP;
	 SigMap sigMap;//sig对应sidToZeroSig中的sid的map
	 std::set<SIGNATURE> sigSet;//sidToZeroSig中的sig集合
	 for (std::set<SNORTID>::iterator it = sidToZeroSig.begin(); it != sidToZeroSig.end(); ++it)
	 {
		 std::set<SIGNATURE> sigs = (SidToSigMap.find(*it))->second;
		 sidToZeroSigMap[*it] = sigs;
		 sidToZeroSigMapTMP[*it] = sigs;
		 for (std::set<SIGNATURE>::iterator j = sigs.begin(); j != sigs.end(); ++j)
		 {
			 sigMap[*j].insert(*it);
			 sigSet.insert(*j);
		 }
	 }

	 //考察余下的所有sid，将这些sid与FSig_SidMap中的sid进行对比，若sid对应的sig不在FSig_SidMap中，则将这个sid分配给该sig,并sidToZeroSigMapTMP中删除
	 //此sid的对应关系。这样可以增加sig的个数
	 for ( SidMap::iterator it = sidToZeroSigMap.begin(); it != sidToZeroSigMap.end(); ++it)
	 {
		 for (std::set<SIGNATURE>::iterator j = it->second.begin(); j != it->second.end(); ++j)
		 {
			 if (!FSig_SidMap.count(*j))
			 {
				 FSig_SidMap[*j].insert(it->first);
				 sidToZeroSigMapTMP.erase(it->first);
				 sigSet.insert(*j);
			 }
		 }
	 }

	 //首先提取sid对应的那些sig中，具有最少分配sid数的那个sig,考察该sig在sigMap中size是否为1,若为1，则将这个sid分配给这个sig,同时在sidToZeroSigMapTMP中
	 //删除该sid的对应关系,加了这部分的结果存于LastStepTest1.txt中，不加这部分的存于LastStep.txt中
	 //for (SidMap::iterator it = sidToZeroSigMapTMP.begin(); it != sidToZeroSigMapTMP.end();)
	 //{
		// SIGNATURE minSig;
		// size_t min = SIZE_MAX;
		// for (std::set<SIGNATURE>::iterator j = it->second.begin(); j != it->second.end(); ++j)
		// {
		//	 size_t nCnt = (FSig_SidMap.find(*j))->second.size();
		//	 if (min > nCnt)
		//	 {
		//		 min = nCnt;
		//		 minSig = *j ;
		//	 }
		// }
		// if ((sigMap.find(minSig))->second.size() == 1)//这个sig只对应sidToZeroSigMap中的一个sid,则将这个sid分配给这个sig 
		// {
		//	 FSig_SidMap[minSig].insert(it->first);
		//	 sidToZeroSig.erase(it->first);
		//	 it = sidToZeroSigMapTMP.erase(it);
		// }
		// else
		//	++it;
	 //}

	 OptimizeInsert(sigSet, sidToZeroSigMapTMP, FSig_SidMap);
 }