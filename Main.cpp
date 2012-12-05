#include "stdafx.h"
#include "MyFun.h"

int main()
{
	std::string strPath = "C:\\test\\Edges.txt";
	std::vector<EDGE> edges;
	Read(edges, strPath);

	system("pause");
	return 0;
}