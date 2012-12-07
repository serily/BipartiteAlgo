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
 void BipartiteMath(SidMap &FSig_SidMap, SidMap &SidToSigMapTmp, std::set<unsigned int> &sidToZeroSig)
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