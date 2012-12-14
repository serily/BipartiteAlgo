#include "stdafx.h"
#include "MyFun.h"

int main()
{
	//std::string strPath = "C:\\test\\Edges.txt";//Snort����
	std::string strPath = "C:\\test\\Content_Pcre\\Content_Pcre_Edges.txt";//Snort����:����content��Pcre
	//std::string strPath = "C:\\test\\URLTest\\URLEdges.txt";//URL����
	std::vector<EDGE> edges;
	Read(edges, strPath);
	std::cout << "Read Edges Complete!" << std::endl;

	SigMap SigToSidMap, SigToSidMapTmp;
	for (std::vector<EDGE>::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		SigToSidMap[it->Sig].insert(it->nSid);
		SigToSidMapTmp[it->Sig].insert(it->nSid);
	}
	std::cout << "Generate Signature to Sids Map Complete!" << std::endl;

	SidMap SidToSigMap, SidToSigMapTmp;
	for (std::vector<EDGE>::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		SidToSigMap[it->nSid].insert(it->Sig);
		SidToSigMapTmp[it->nSid].insert(it->Sig);
	}
	std::cout << "Generate Sid to Signature Map Complete!" << std::endl;

	//sig to sid
	//std::ofstream fout1("C:\\test\\Sig_Sid_Map.txt");
	//for (SigMap::iterator it = SigToSidMap.begin(); it != SigToSidMap.end(); ++it)
	//{
	//	fout1 << it->first << "\t";
	//	for (std::set<SNORTID>::iterator i = it->second.begin(); i != it->second.end(); ++i)
	//	{
	//		fout1 << *i << " ";
	//	}
	//	fout1 << std::endl;
	//}
	//fout1.close();

	////sid to sig
	//std::ofstream fout2("C:\\test\\Sid_Sig_Map.txt");
	//for (SidMap::iterator it = SidToSigMap.begin(); it != SidToSigMap.end(); ++it)
	//{
	//	fout2 << it->first << "\t";
	//	for (std::set<SIGNATURE>::iterator i = it->second.begin(); i != it->second.end(); ++i)
	//	{
	//		fout2 << *i << " ";
	//	}
	//	fout2 << std::endl;
	//}
	//fout2.close();

	SigMap FSig_SidMap; 
	std::set<unsigned int> sidToZeroSig;

	std::cout << "First Step: Extract sid only to one signature and signature only has one sid." << std::endl;
	InitialFind(SidToSigMap, SidToSigMapTmp, SigToSidMap, FSig_SidMap, sidToZeroSig);
	std::cout << "Extract sid only to one signature and signature only to one sid complete! Details in FirstStep.txt" << std::endl << std::endl;
	//std::cout << "Now there are " << sidToZeroSig.size() << " Sids have zero signature temporarily." << std::endl;


	//One sid only to one sig and one sig only to one sid, FirstStep.txt
	//std::ofstream fout1("C:\\test\\FirstStep.txt");
	//unsigned int nCntSig = FSig_SidMap.size(), nCntSid = 0;
	//fout1 << "Signature" << "\t" << "SidCounts" << "\t" << "Sids" << std::endl;
	//for (SigMap::iterator it = FSig_SidMap.begin(); it != FSig_SidMap.end(); ++it)
	//{
	//	nCntSid += it->second.size();
	//	fout1 << it->first << "\t";
	//	fout1 << it->second.size() << "\t";
	//	for (std::set<SNORTID>::iterator i = it->second.begin(); i != it->second.end(); ++i)
	//	{
	//		fout1 << *i << " ";
	//	}
	//	fout1 << std::endl;
	//}
	//fout1 << "��" << nCntSig << "��signature��" << nCntSid << "��sid";
	//fout1.close();

	std::cout << std::endl << "Second Step: Maximum Binary Matching" << std::endl;
	BipartiteMath(FSig_SidMap, SidToSigMapTmp, sidToZeroSig);

	//std::ofstream fout2("C:\\test\\SecondStep.txt");
	//unsigned int nCntSig = FSig_SidMap.size(), nCntSid = 0;
	//fout2 << "Signature" << "\t" << "SidCounts" << "\t" << "Sids" << std::endl;
	//for (SigMap::iterator it = FSig_SidMap.begin(); it != FSig_SidMap.end(); ++it)
	//{
	//	nCntSid += it->second.size();
	//	fout2 << it->first << "\t";
	//	fout2 << it->second.size() << "\t";
	//	for (std::set<SNORTID>::iterator i = it->second.begin(); i != it->second.end(); ++i)
	//	{
	//		fout2 << *i << " ";
	//	}
	//	fout2 << std::endl;
	//}
	//fout2 << "��" << nCntSig << "��signature��" << nCntSid << "��sid";
	//fout2.close();

	std::cout << "Second Step Complete!" << std::endl;
	std::cout << std::endl << "Last Step: Insert the remaining Sids" << std::endl;
	InsertRemain(FSig_SidMap, SidToSigMap, SigToSidMap, sidToZeroSig);
	std::cout << std::endl << "Last Step Complete!" << std::endl;

	//std::ofstream fout3("C:\\test\\LastStepTest3.txt");//laststep3.txt�ǲ�������ΰ���㷨�ĵ��Ĳ���content��pcre
	std::ofstream fout3("C:\\test\\Content_Pcre\\LastStepTest3.txt");//laststep3.txt�ǲ�������ΰ���㷨�ĵ��Ĳ���laststep4.txt�������Ĳ�,���ý�����в�ͬ
	//std::ofstream fout3("C:\\test\\URLTest\\URLLastStepTest3.txt");//laststep3.txt�ǲ�������ΰ���㷨�ĵ��Ĳ���laststep4.txt�������Ĳ�,���ý�����в�ͬ
	unsigned int nCntSig = FSig_SidMap.size(), nCntSid = 0;
	std::vector<size_t> sidCnt;
	fout3 << "Signature" << "\t" << "SidCounts" << "\t" << "Sids" << std::endl;
	for (SigMap::iterator it = FSig_SidMap.begin(); it != FSig_SidMap.end(); ++it)
	{
		sidCnt.push_back(it->second.size());
		nCntSid += it->second.size();
		fout3 << it->first << "\t";
		fout3 << it->second.size() << "\t";
		for (std::set<SNORTID>::iterator i = it->second.begin(); i != it->second.end(); ++i)
		{
			fout3 << *i << " ";
		}
		fout3 << std::endl;
	}
	fout3 << "��" << nCntSig << "��signature��" << nCntSid << "��sid";
	fout3 << std::endl;
	fout3 << "sid����" << "\t" << "signature����" << std::endl;
	sort(sidCnt.begin(), sidCnt.end());
	std::vector<size_t>::iterator iPrev = sidCnt.begin();
	size_t count = 0;
	std::vector<size_t>::iterator iter = sidCnt.begin();
	for (; iter != sidCnt.end(); ++iter)
	{
		++count;
		if (*iter != *iPrev)
		{
			--count;
			fout3 << *iPrev << "\t" << count << std::endl;
			iPrev = iter;
			count = 1;
		}
	}
	if (iter == sidCnt.end())
	{
		fout3 << *iPrev << "\t" << count << std::endl;
	}

	fout3.close();

	system("pause");
	return 0;
}