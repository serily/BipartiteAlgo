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


//function declaration
 void Read(std::vector<EDGE> &edges, std::string &strPath);