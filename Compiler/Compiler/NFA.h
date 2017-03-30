#pragma once

using namespace std;

typedef struct eNFAs
{
	int state;
	int edgeNum[10];
	bool isAccept;
	struct eNFAs* left;
	struct eNFAs* right;
}eNFA;

bool isValidRE(std::string str);
eNFA* RE2eNFA(string S);
