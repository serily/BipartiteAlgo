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
		/** ö���ұߵĵ� */	
		for(size_t i = 1; i <= rNum; i++)	
		{	
			/** ����������·Ѱ���ұߵ������û�����ʣ�������ߵ�v��ͨ */	
			/*��������ߵĵ���ͨ�������ұߵĵ�*/
			if(!vis[i - 1] && map[v - 1][i - 1])	
			{	
				vis[i - 1] = 1;	
 	
				/**	
				 * dfs(match[i], n)���ұߵ�i���ƥ�䶥��Ϊmatch[i]	
				 * ˵����������ƥ��ߣ���ô���������ѿ����ܲ����������·	
				 * ������������·�������ǽ�i���ƥ�䶥���޸�Ϊv	
				 * ���match[i]Ϊ0��˵��i����δ��ƥ��ĵ�	
				 * ��ô��i���ƥ�䶥���޸�Ϊv	
				 * ��δ�������ˡ�ƥ��->δƥ�䡱��������	
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
    /** ö����ߵĵ� */	
    for(size_t i = 1; i <= lNum; i++)	
    {	
        /** ���³�ʼ��vis */	
		fill(vis.begin(), vis.end(), 0);
 	
        /** ����ߵ�i��������· */	
        dfs(i, rNum, map, match, vis);	
    }	
}
//Second Step: BipartiteMathing
 void BipartiteMath(SidMap &FSig_SidMap, SidMap &SidToSigMapTmp, std::set<SNORTID> &sidToZeroSig)
 {
	size_t lNum = 0;
	size_t rNum = 0;
	std::map<SNORTID, unsigned int> mSidInt;//sid��map�������Ķ�Ӧ��ϵ
	std::map<SIGNATURE, unsigned int> mSigInt;//sig��map�������Ķ�Ӧ��ϵ
	std::set<SIGNATURE> sigTmpSet;
	std::set<SNORTID> sidTmpSet;
	//��SidToSigMapTmp�е�sid��sig�Ķ�Ӧ��ϵ������ʱmapTmp��
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
		unsigned int sid = (mSidInt.find(i->first))->second;//sid��Ӧ�����ͱ���
		for (std::set<SIGNATURE>::iterator j = i->second.begin(); j != i->second.end(); ++j)
		{
			unsigned int sig = (mSigInt.find(*j))->second;//sig��Ӧ�����ͱ���
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

	//����������ƥ��
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

	std::map<unsigned int, SNORTID> mIntSid;//���ͱ�����sid�Ķ�Ӧ��ϵ
	std::map<unsigned int, SIGNATURE> mIntSig;//���ͱ�����sig�Ķ�Ӧ��ϵ
	for (std::map<SNORTID, unsigned int>::iterator it = mSidInt.begin(); it != mSidInt.end(); ++it)
	{
		mIntSid[it->second] = it->first;
	}
	for (std::map<SIGNATURE, unsigned int>::iterator it = mSigInt.begin(); it != mSigInt.end(); ++it)
	{
		mIntSig[it->second] = it->first;
	}

	//������ƥ��Ľ������FSig_SidMap��
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

	std::cout << std::endl << "������ƥ���ҵ���" << matchCnt << "��sid��sigһ��һ��ϵ.��ʣ" << SidToSigMapTmp.size() 
		<< "��sidδ����" << std::endl;

	//��ʣ���δ�����sid����sidToZeroSig��
	for (SigMap::iterator it = SidToSigMapTmp.begin(); it != SidToSigMapTmp.end(); ++it)
	{
		sidToZeroSig.insert(it->first);
	}
 }

void OptimizeInsert(std::set<SIGNATURE> &sigSet, std::map<SNORTID, std::set<SIGNATURE>> &sidToZeroSigMapTMP, 
					SigMap &FSig_SidMap)
{
	 std::map<SIGNATURE, size_t> sigTosidCnt;//�Ѿ������sig��Ӧ��sid����
	 for (std::set<SIGNATURE>::iterator it = sigSet.begin(); it != sigSet.end(); ++it)
	 {
		 size_t nCnt = (FSig_SidMap.find(*it))->second.size();
		 sigTosidCnt[*it] = nCnt;
	 }

	 std::map<SNORTID, SIGNATURE> mapTmp;//sid��Ӧsig����ʱmap
	 SigMap mapTmpSet;//sig��Ӧsid���ϵ���ʱmap
	 bool flag = true;//�Ƿ�����˵���
	 while (flag)
	 {
		 flag = false;
		 for (SidMap::iterator it = sidToZeroSigMapTMP.begin(); it != sidToZeroSigMapTMP.end(); ++it)
		 {
			 if (mapTmp.count(it->first) == 0)//����sid��û�з���signature,�Ͱѵ�ǰ������صľ�������sid����signature�������
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
			 else//��sid�Ѿ����䣬���Ƿ��ܽ��е���
			 {
				 SIGNATURE assignSig = mapTmp[it->first];//��sid�����sig
				 size_t num = (sigTosidCnt.find(assignSig))->second;//assginSig��ǰ���е�sid����
				 size_t min = SIZE_MAX;//��sid��Ӧ�ĵ�ǰ������С����sid��sig
				 SIGNATURE sig;//
				 for (std::set<SIGNATURE>::iterator j = it->second.begin(); j != it->second.end(); ++j)
				 {
					 if (min > (sigTosidCnt.find(*j))->second)
					 {
						 min = (sigTosidCnt.find(*j))->second;
						 sig = *j;
					 }
				 }
				 if ((min + 1) < num)//����sidֱ�ӷ����sig���������assginSig�İ󶨹�ϵ
				 {
					 --(sigTosidCnt.find(assignSig))->second;
					 mapTmp[it->first] = sig;
					 mapTmpSet[sig].insert(it->first);
					 ++(sigTosidCnt.find(sig))->second;
					 flag = true;
				 }
				 //����sig��Ӧ������sid����Щsid�У�sid2��Ӧ�ľ�������sid������sig2,������sig2��Ӧ�ĵ�sid������minС��
				 //���sid2�ָ�sig2,Ȼ���sid�ָ�sig������ΰ�������ĵ��Ĳ�����������һ���Ľ������Laststep3.txt�������Ĵ���LastStep4.txt��
				 else if ((min + 1) == num)
				 {
					 SIGNATURE sigFirstLevel, sigSecondLevel;
					 SNORTID sidSecond;
					 size_t secondMin = min;
					 bool change = false;

					 std::set<SNORTID> sidCntTmp;//��ǰ�����sig��Ӧ��sidToZeroSig�е�sid����
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
							 for (std::set<SNORTID>::iterator h = mapTmpSet[*j].begin(); h != mapTmpSet[*j].end(); ++h)//����ÿ��sid��Ӧ��sig
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
						 //��sidSecond���
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
		//	 bool mark = false;//��sid��δ����
		//	 if (tmp != 0)
		//	 {
		//		 mark = true;//��sid�Ѿ��������
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
		//	 if (sigInitCnt == 0)//��sid��û�з����
		//	 {
		//		 mapTmp[i->first] = sig;
		//		 ++(sigTosidCnt.find(sig))->second;
		//		 flag = true;
		//	 }
		//	 else
		//	 {
		//		 SIGNATURE sigInit = mapTmp[i->first];
		//		 if (sigInit != sig)//�����˵���
		//		 {
		//			 mapTmp[i->first] = sig;
		//			 ++(sigTosidCnt.find(sig))->second;
		//			 --(sigTosidCnt.find(sigInit))->second;
		//			 flag = true;
		//		 }
		//	 }
		// }
	 //}

	 //��mapTmp�з���FSig_SidMap��
	 for (std::map<SNORTID, SIGNATURE>::iterator it = mapTmp.begin(); it != mapTmp.end(); ++it)
	 {
		 FSig_SidMap[it->second].insert(it->first);
	 }
}

 void InsertRemain(SigMap &FSig_SidMap, SidMap &SidToSigMap, SigMap &SigToSidMap, std::set<SNORTID> &sidToZeroSig)
 {
	 SidMap sidToZeroSigMap;
	 SidMap sidToZeroSigMapTMP;
	 SigMap sigMap;//sig��ӦsidToZeroSig�е�sid��map
	 std::set<SIGNATURE> sigSet;//sidToZeroSig�е�sig����
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

	 //�������µ�����sid������Щsid��FSig_SidMap�е�sid���жԱȣ���sid��Ӧ��sig����FSig_SidMap�У������sid�������sig,��sidToZeroSigMapTMP��ɾ��
	 //��sid�Ķ�Ӧ��ϵ��������������sig�ĸ���
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

	 //������ȡsid��Ӧ����Щsig�У��������ٷ���sid�����Ǹ�sig,�����sig��sigMap��size�Ƿ�Ϊ1,��Ϊ1�������sid��������sig,ͬʱ��sidToZeroSigMapTMP��
	 //ɾ����sid�Ķ�Ӧ��ϵ,�����ⲿ�ֵĽ������LastStepTest1.txt�У������ⲿ�ֵĴ���LastStep.txt��
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
		// if ((sigMap.find(minSig))->second.size() == 1)//���sigֻ��ӦsidToZeroSigMap�е�һ��sid,�����sid��������sig 
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