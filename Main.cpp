#include "stdafx.h"
#include "MyFun.h"

int main()
{
	std::string strPath = "C:\\test\\Edges.txt";
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

	SigMap SidtoOneSigMap;
	std::set<unsigned int> sidToZeroSig;

	FindSidToOneSig(SidToSigMap, SidToSigMapTmp, SidtoOneSigMap, sidToZeroSig);


	//One sid only to one sig, Sid-OneSig_zheng.txt
	//std::ofstream fout1("C:\\test\\Sid-OneSig_zheng.txt");
	//unsigned int nCntSig = SidtoOneSigMap.size(), nCntSid = 0;
	//fout1 << "Signature" << "\t" << "SidCounts" << "\t" << "Sids" << std::endl;
	//for (SigMap::iterator it = SidtoOneSigMap.begin(); it != SidtoOneSigMap.end(); ++it)
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
	//fout1 << "共" << nCntSig << "个signature和" << nCntSid << "个sid";
	//fout1.close();



	system("pause");
	return 0;
}